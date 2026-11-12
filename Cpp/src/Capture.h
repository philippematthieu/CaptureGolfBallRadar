//============================================================================
// Capture.h — lecture captation radar (CSV 2 lignes ';' ou WAV stéréo) et
// estimation simplifiée : vBall (ridge STFT post-impact), spin total
// (périodicité du patch via enveloppe de Hilbert), alpha0 (appariement stéréo).
// Header-only, C++11, aucune dépendance externe (FFT radix-2 itérative).
//============================================================================
#ifndef CAPTURE_H_
#define CAPTURE_H_
#if defined(__GNUC__)
#define CAP_UNUSED __attribute__((unused))
#else
#define CAP_UNUSED
#endif

#include <vector>
#include <string>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <algorithm>

#ifdef __APPLE__
#include <AudioToolbox/AudioToolbox.h>   // mode --live : entrée carte son (radar en mode audio)
#include <unistd.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static const double KV = 19.49;        // Hz / (km/h) radar 10.525 GHz (f_D = KV * V_kmh)
static const double HP_KMH = 50.0;     // passe-haut avant spectrogramme d'affichage (km/h) : rejette le lobe du club (comme plot_spectro.py --hp 50)
static const double VMAX_KMH = 250.0;  // vue pleine spectrogramme : axe Y 0..250 km/h
static const double KS = 0.1567;       // Hz / RPM (excursion micro-Doppler, d = 0.04267 m, doc §10)
static const double BALL_D = 0.04267;  // diamètre balle (m), doc §10/§2
static const double FS = 44100.0;
static const double B_BASE = 0.30;     // baseline radars (m)
static const double L0 = 1.50;         // distance initiale balle (m)
static const double THETA_RADAR = 20.0 * M_PI / 180.0;  // biais géométrique radar (corrigé après)

// interpolation sous-bin parabolique du pic (comme pipe_validation.parabolic_peak)
CAP_UNUSED static double parabolic_peak(const std::vector<double>& P, int k) {
    if (k <= 0 || k >= (int)P.size() - 1) return (double)k;
    double den = P[k-1] - 2.0*P[k] + P[k+1];
    if (den == 0.0) return (double)k;
    double d = 0.5 * (P[k-1] - P[k+1]) / den;
    return k + (std::fabs(d) <= 1.0 ? d : 0.0);
}

// ---------------- FFT itérative radix-2 (longueur = puissance de 2) --------
CAP_UNUSED static void fft(std::vector<double>& re, std::vector<double>& im, bool inverse) {
    const int n = (int)re.size();
    for (int i = 1, j = 0; i < n; i++) {          // bit-reversal
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) { std::swap(re[i], re[j]); std::swap(im[i], im[j]); }
    }
    for (int len = 2; len <= n; len <<= 1) {
        double ang = 2.0 * M_PI / len * (inverse ? 1.0 : -1.0);
        double wr = cos(ang), wi = sin(ang);
        for (int i = 0; i < n; i += len) {
            double cwr = 1.0, cwi = 0.0;
            for (int j = 0; j < len / 2; j++) {
                double ur = re[i + j], ui = im[i + j];
                double vr = re[i + j + len / 2] * cwr - im[i + j + len / 2] * cwi;
                double vi = re[i + j + len / 2] * cwi + im[i + j + len / 2] * cwr;
                re[i + j] = ur + vr; im[i + j] = ui + vi;
                re[i + j + len / 2] = ur - vr; im[i + j + len / 2] = ui - vi;
                double nwr = cwr * wr - cwi * wi;
                cwi = cwr * wi + cwi * wr; cwr = nwr;
            }
        }
    }
    if (inverse) for (int i = 0; i < n; i++) { re[i] /= n; im[i] /= n; }
}

// ---------------- passe-haut FFT (brique) : zero les bins |f| < fcut ----------
CAP_UNUSED static std::vector<double> highpass(const std::vector<double>& x, double fcut) {
    int n = (int)x.size();
    int N = 1; while (N < n) N <<= 1;                 // FFT radix-2 : zero-pad
    std::vector<double> re(N, 0.0), im(N, 0.0);
    double m = 0; for (int i = 0; i < n; i++) m += x[i]; m /= n;
    for (int i = 0; i < n; i++) re[i] = x[i] - m;
    fft(re, im, false);
    for (int k = 0; k < N; k++) {
        double f = (k <= N / 2) ? k * FS / N : (k - N) * FS / N;
        if (std::fabs(f) < fcut) { re[k] = 0.0; im[k] = 0.0; }
    }
    fft(re, im, true);
    std::vector<double> y(n);
    for (int i = 0; i < n; i++) y[i] = re[i];
    return y;
}

// déclaration anticipée (définie plus bas) : utilisée par estimate() pour la voie patch
CAP_UNUSED static std::vector<double> bandpass(const std::vector<double>& x, double f0, double bw);

// ---------------- chargement ------------------------------------------------
struct Capture {
    std::vector<double> r1, r2;   // canaux R1/R2
    double fs = FS;
};

CAP_UNUSED static Capture load_capture(const std::string& path) {
    Capture c;
    std::string low = path;
    std::transform(low.begin(), low.end(), low.begin(), ::tolower);
    if (low.size() >= 4 && low.compare(low.size() - 4, 4, ".wav") == 0) {
        FILE* f = fopen(path.c_str(), "rb");
        if (!f) { fprintf(stderr, "ERREUR: %s introuvable\n", path.c_str()); exit(2); }
        unsigned char hdr[44];
        if (fread(hdr, 1, 44, f) != 44) { fclose(f); exit(2); }
        int ch = hdr[22] | (hdr[23] << 8);
        int bits = hdr[34] | (hdr[35] << 8);
        c.fs = hdr[24] | (hdr[25] << 8) | (hdr[26] << 16) | (hdr[27] << 24);
        // data chunk
        unsigned char sz[4];
        long pos = 44;
        while (pos < 1 << 20) {
            fseek(f, pos, SEEK_SET);
            if (fread(sz, 1, 4, f) != 4) break;
            unsigned dsize = sz[0] | (sz[1] << 8) | (sz[2] << 16) | (sz[3] << 24);
            char id[5] = {0};
            fseek(f, pos + 4, SEEK_SET);
            if (fread(id, 1, 4, f) != 4) break;
            if (memcmp(id, "data", 4) == 0) { fseek(f, pos + 8, SEEK_SET); break; }
            pos += 8 + dsize;
        }
        long n = (long)(c.fs * 4); // ~4 s max
        std::vector<double> data;
        if (bits == 16) {
            std::vector<int16_t> buf(n * ch);
            long got = (long)fread(buf.data(), 2, n * ch, f);
            for (long i = 0; i < got; i++) data.push_back(buf[i] / 32768.0);
        } else if (bits == 8) {
            std::vector<uint8_t> buf(n * ch);
            long got = (long)fread(buf.data(), 1, n * ch, f);
            for (long i = 0; i < got; i++) data.push_back(buf[i] / 128.0 - 1.0);
        } else { // 32 bits float ou int
            std::vector<int32_t> buf(n * ch);
            long got = (long)fread(buf.data(), 4, n * ch, f);
            for (long i = 0; i < got; i++) data.push_back(buf[i] / 2147483648.0);
        }
        fclose(f);
        long nfr = (long)data.size() / ch;
        c.r1.resize(nfr); c.r2.resize(nfr);
        for (long i = 0; i < nfr; i++) {
            c.r1[i] = data[i * ch];
            c.r2[i] = (ch > 1) ? data[i * ch + 1] : data[i * ch];
        }
    } else { // CSV 2 lignes ';'
        std::ifstream in(path.c_str());
        if (!in) { fprintf(stderr, "ERREUR: %s introuvable\n", path.c_str()); exit(2); }
        std::string line;
        int row = 0;
        while (std::getline(in, line) && row < 2) {
            std::vector<double>& dst = (row == 0) ? c.r1 : c.r2;
            std::stringstream ss(line);
            std::string tok;
            while (std::getline(ss, tok, ';')) {
                if (!tok.empty()) dst.push_back(strtod(tok.c_str(), NULL));
            }
            row++;
        }
        if (c.r2.empty()) c.r2 = c.r1;
    }
    return c;
}

// ---------------- sauvegarde WAV 16 bits (pour relecture Python : parité) ------
CAP_UNUSED static void save_wav(const std::string& path, const Capture& c) {
    FILE* f = fopen(path.c_str(), "wb");
    if (!f) { fprintf(stderr, "ERREUR: impossible d'écrire %s\n", path.c_str()); return; }
    long nfr = (long)std::min(c.r1.size(), c.r2.size());
    uint32_t dataSize = (uint32_t)(nfr * 2 * 2), fs = (uint32_t)c.fs;
    uint16_t ch = 2, bits = 16, blockAlign = 4; uint32_t byteRate = fs * 4;
    fwrite("RIFF", 1, 4, f); uint32_t riffSize = 36 + dataSize; fwrite(&riffSize, 4, 1, f);
    fwrite("WAVEfmt ", 1, 8, f); uint32_t fmtSize = 16; fwrite(&fmtSize, 4, 1, f);
    uint16_t audioFmt = 1; fwrite(&audioFmt, 2, 1, f); fwrite(&ch, 2, 1, f);
    fwrite(&fs, 4, 1, f); fwrite(&byteRate, 4, 1, f); fwrite(&blockAlign, 2, 1, f); fwrite(&bits, 2, 1, f);
    fwrite("data", 1, 4, f); fwrite(&dataSize, 4, 1, f);
    for (long i = 0; i < nfr; i++) {
        int16_t l = (int16_t)std::max(-32768.0, std::min(32767.0, c.r1[i] * 32767.0));
        int16_t r = (int16_t)std::max(-32768.0, std::min(32767.0, c.r2[i] * 32767.0));
        fwrite(&l, 2, 1, f); fwrite(&r, 2, 1, f);
    }
    fclose(f);
}

// ---------------- entrée live carte son (macOS, CoreAudio AudioQueue) ----------
#ifdef __APPLE__
static void capture_live_cb(void* ud, AudioQueueRef, AudioQueueBufferRef inBuffer, const AudioTimeStamp*, UInt32, const AudioStreamPacketDescription*) {
    std::vector<float>* b = (std::vector<float>*)ud;
    const float* p = (const float*)inBuffer->mAudioData;
    UInt32 n = inBuffer->mAudioDataByteSize / sizeof(float);
    b->insert(b->end(), p, p + n);
}
#endif
// Enregistre `seconds` s sur l'entrée par défaut, stéréo 44,1 kHz float32.
CAP_UNUSED static Capture record_live(double seconds) {
    Capture c; c.fs = 44100.0;
#ifdef __APPLE__
    AudioStreamBasicDescription fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.mSampleRate = 44100.0;
    fmt.mFormatID = kAudioFormatLinearPCM;
    fmt.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked;
    fmt.mBitsPerChannel = 32;
    fmt.mChannelsPerFrame = 2;
    fmt.mFramesPerPacket = 1;
    fmt.mBytesPerFrame = 2 * 4;
    fmt.mBytesPerPacket = 2 * 4;
    std::vector<float> buf;
    AudioQueueRef queue = NULL;
    OSStatus st = AudioQueueNewInput(&fmt, capture_live_cb, &buf, NULL, NULL, 0, &queue);
    if (st != noErr) { fprintf(stderr, "ERREUR AudioQueueNewInput (%d) — périphérique audio absent ?\n", (int)st); return c; }
    UInt32 bufBytes = (UInt32)(0.5 * 44100.0 * 2 * 4);   // buffers internes 0,5 s
    for (int i = 0; i < 3; i++) {
        AudioQueueBufferRef bbuf;
        if (AudioQueueAllocateBuffer(queue, bufBytes, &bbuf) == noErr)
            AudioQueueEnqueueBuffer(queue, bbuf, 0, NULL);
    }
    st = AudioQueueStart(queue, NULL);
    if (st != noErr) { fprintf(stderr, "ERREUR AudioQueueStart (%d)\n", (int)st); AudioQueueDispose(queue, true); return c; }
    fprintf(stderr, "enregistrement live %.1f s sur l'entrée par défaut (44,1 kHz stéréo) — frappez !\n", seconds);
    usleep((useconds_t)(seconds * 1e6));
    AudioQueueStop(queue, true);
    AudioQueueDispose(queue, true);
    long nfr = (long)buf.size() / 2;
    c.r1.resize(nfr); c.r2.resize(nfr);
    for (long i = 0; i < nfr; i++) { c.r1[i] = buf[2 * i]; c.r2[i] = buf[2 * i + 1]; }
    fprintf(stderr, "live : %ld frames stéréo capturées\n", nfr);
#else
    (void)seconds;
    fprintf(stderr, "ERREUR: --live (carte son) nécessite macOS (CoreAudio). Compilez sur mac avec -framework AudioToolbox.\n");
#endif
    return c;
}

// ---------------- spectrogramme + ridge --------------------------------------
CAP_UNUSED static void spectrogram(const std::vector<double>& x, int nfft, int hop,
                        std::vector<double>& ridge, std::vector<double>& power,
                        std::vector<double>& tgrid,
                        double flo = 1000.0, double fhi = 6000.0) {
    int n = (int)x.size();
    int nframes = 1 + (n - nfft) / hop;
    std::vector<double> w(nfft);
    for (int i = 0; i < nfft; i++) w[i] = 0.5 - 0.5 * cos(2.0 * M_PI * i / (nfft - 1)); // Hann
    ridge.assign(nframes, 0.0); power.assign(nframes, 0.0); tgrid.assign(nframes, 0.0);
    std::vector<double> re(nfft), im(nfft);
    for (int fr = 0; fr < nframes; fr++) {
        for (int i = 0; i < nfft; i++) { re[i] = x[fr * hop + i] * w[i]; im[i] = 0.0; }
        fft(re, im, false);
        std::vector<double> P(nfft / 2);
        int kmax = 1; double pmax = 0.0;
        for (int k = 1; k < nfft / 2; k++) {
            double f = k * FS / nfft;
            P[k] = re[k] * re[k] + im[k] * im[k];
            if (f < flo || f > fhi) { P[k] = -1.0; continue; }  // passe-bande vitesse (balle par défaut)
            if (P[k] > pmax) { pmax = P[k]; kmax = k; }
        }
        ridge[fr] = parabolic_peak(P, kmax) * FS / nfft;   // sous-bin (comme Python)
        power[fr] = pmax;
        tgrid[fr] = (fr * hop + nfft / 2.0) / FS;
    }
}

// ---------------- estimation (conforme pipe_validation.py) -------------------
struct EstRadar {
    double vball_kmh;      // radiale
    double vball_corr_kmh; // corrigée cos(20°) (et cos(case angle) si --caseangle)
    double case_deg = 0;   // angle d'inclinaison de la valise/radar vers le haut (--caseangle)
    double spin_rpm;       // périodicité du patch
    double spin_peigne_rpm = 0; // peigne de raies micro-Doppler (autocorrélation du spectre autour de f_D)
    double spin_phase_rpm = 0;  // fréquence instantanée du signal analytique du lobe balle
    double spin_exc_rpm = 0;    // excursion micro-Doppler : spin = Δf crête-crête / (2·KS), β = 4πr/λ
    double spin_patch_conf = 0;  // confiance voie patch   = pic / médiane spectre enveloppe (bande 50-200 Hz)
    double spin_phase_conf = 0;  // confiance voie phase   = pic / médiane spectre fi detrendée (40-200 Hz)
    double spin_peigne_conf = 0; // confiance voie peigne  = ac[best] / médiane ac (lags 45-200 Hz)
    double spin_exc_conf = 0;    // confiance voie excursion = Δf / (1.4826·MAD résidu fi lissée)
    double alpha0_deg;     // azimut initial (appariement stéréo)
    double dalpha_deg;     // courbure début-fin
    double t0_s;
    double launch_deg = 0;   // angle de lancement ψ estimé par fit balistique sur le ridge post-impact
    double launch_conf = 0;  // confiance du fit = 1 − SSE/SST
    // spectrogramme embarqué pour affichage fixe (densité 0..255, bande FMIN..FMAX Hz, axe Y en km/h via /KV)
    std::vector<unsigned char> spec;   // [nframes * nbin], ligne = frame temporelle
    int spec_nbin = 0, spec_nframe = 0, spec_hop = 0;
    double spec_fmin = 0, spec_fmax = 0;
    std::vector<double> spec_ridge;    // Hz par frame
    std::vector<double> spec_power;    // puissance max par frame (0..1 normalisée)
    int spec_i0 = 0;                   // frame impact
};

CAP_UNUSED static void fill_spec(EstRadar& e, const std::vector<double>& sig, int NFFT, int HOP) {
        // conforme plot_spectro.py : passe-haut 50 km/h, vue pleine 0..250 km/h,
        // ridge = crête pleine bande par frame (interpolation parabolique, comme Python)
        const double FMIN = 0.0, FMAX = VMAX_KMH * KV;      // 0..4872,5 Hz
        std::vector<double> s = highpass(sig, HP_KMH * KV); // passe-haut 974,5 Hz (50 km/h)
        int nframes = 1 + ((int)s.size() - NFFT) / HOP;
        int k0 = (int)(FMIN * NFFT / FS), k1 = (int)(FMAX * NFFT / FS);
        if (k1 >= NFFT / 2) k1 = NFFT / 2 - 1;
        e.spec_nbin = k1 - k0 + 1;
        e.spec_nframe = nframes;
        e.spec_hop = HOP;
        e.spec_fmin = FMIN; e.spec_fmax = FMAX;
        e.spec.resize((size_t)nframes * e.spec_nbin);
        e.spec_ridge.assign(nframes, 0.0);
        e.spec_power.assign(nframes, 0.0);
        std::vector<double> w(NFFT), re(NFFT), im(NFFT), P(NFFT / 2);
        for (int i = 0; i < NFFT; i++) w[i] = 0.5 - 0.5 * cos(2.0 * M_PI * i / (NFFT - 1));
        // passe 1 : p99 pour la normalisation dB
        std::vector<double> allp;
        for (int fr = 0; fr < nframes; fr++) {
            for (int i = 0; i < NFFT; i++) { re[i] = s[fr * HOP + i] * w[i]; im[i] = 0; }
            fft(re, im, false);
            for (int k = k0; k <= k1; k++) allp.push_back(re[k] * re[k] + im[k] * im[k]);
        }
        std::sort(allp.begin(), allp.end());
        double gmax = allp.empty() ? 1.0 : allp[(size_t)(allp.size() * 0.99)];  // p99 comme matplotlib
        if (gmax <= 0) gmax = 1;
        // passe 2 : image dB [-80..0] + ridge pleine bande + puissance
        for (int fr = 0; fr < nframes; fr++) {
            for (int i = 0; i < NFFT; i++) { re[i] = s[fr * HOP + i] * w[i]; im[i] = 0; }
            fft(re, im, false);
            for (int k = 0; k < NFFT / 2; k++) P[k] = re[k] * re[k] + im[k] * im[k];
            int kb = 1; double pmax = 0;
            for (int k = 1; k < NFFT / 2; k++) if (P[k] > pmax) { pmax = P[k]; kb = k; }
            e.spec_ridge[fr] = parabolic_peak(P, kb) * FS / NFFT;
            e.spec_power[fr] = pmax / gmax;
            for (int k = k0; k <= k1; k++) {
                double db = 10.0 * log10(P[k] / gmax + 1e-12);
                if (db < -80.0) db = -80.0;
                e.spec[(size_t)fr * e.spec_nbin + (k - k0)] = (unsigned char)(255.0 * (db + 80.0) / 80.0);
            }
        }
        int i0l = 0; for (size_t i = 1; i < e.spec_power.size(); i++) if (e.spec_power[i] > e.spec_power[i0l]) i0l = (int)i;
        e.spec_i0 = i0l;
        // dès l'impact, le ridge AFFICHÉ est cherché dans [0,85..1,10]·f_D : la ligne cyan colle
        // à la balle juste après l'impact (sinon elle suivrait l'écho club, très fort, qui décélère).
        // Avant l'impact on garde le ridge pleine bande (on voit le lobe club monter).
        if (e.vball_kmh > 0 && i0l < nframes) {
            double fD = e.vball_kmh * KV;
            int klo = std::max(1, (int)(fD * 0.85 * NFFT / FS));
            int khi = std::min(NFFT / 2 - 1, (int)(fD * 1.10 * NFFT / FS));
            int frEnd = std::min(nframes, i0l + (int)(0.5 * FS / HOP));   // ~0,5 s post-impact
            for (int fr = i0l; fr < frEnd; fr++) {
                for (int i = 0; i < NFFT; i++) { re[i] = s[fr * HOP + i] * w[i]; im[i] = 0; }
                fft(re, im, false);
                for (int k = 0; k < NFFT / 2; k++) P[k] = re[k] * re[k] + im[k] * im[k];
                int kb = klo; double pmax = 0;
                for (int k = klo; k <= khi; k++) if (P[k] > pmax) { pmax = P[k]; kb = k; }
                e.spec_ridge[fr] = parabolic_peak(P, kb) * FS / NFFT;
            }
        }
}

CAP_UNUSED static EstRadar estimate(const Capture& c, double caseDeg = 0.0) {
    EstRadar e; e.vball_kmh = 0; e.vball_corr_kmh = 0; e.spin_rpm = 0; e.alpha0_deg = 0; e.dalpha_deg = 0; e.t0_s = 0;
    e.case_deg = caseDeg;
    const double caseRad = caseDeg * M_PI / 180.0;   // inclinaison de la valise vers le haut (--caseangle)
    const int NFFT = 2048, HOP = NFFT / 2;
    std::vector<double> ridge, power, tg;
    spectrogram(c.r1, NFFT, HOP, ridge, power, tg);
    // impact = frame max énergie
    int i0 = 0; for (size_t i = 1; i < power.size(); i++) if (power[i] > power[i0]) i0 = (int)i;
    e.t0_s = tg[i0];
    // vBall = médiane du ridge post-impact (50 ms) sur CHAQUE canal, puis rejet inter-radars > 15 %
    // (comme pipe_validation : si les 2 canaux divergent = faux lobe, on garde la médiane des 2)
    auto vball_canal = [&](const std::vector<double>& x) -> double {
        std::vector<double> r2, p2, t2;
        spectrogram(x, NFFT, HOP, r2, p2, t2);
        int j0 = 0; for (size_t i = 1; i < p2.size(); i++) if (p2[i] > p2[j0]) j0 = (int)i;
        std::vector<double> post;
        for (size_t i = j0; i < r2.size() && t2[i] < t2[j0] + 0.05; i++) post.push_back(r2[i]);
        if (post.empty()) return 0.0;
        std::sort(post.begin(), post.end());
        return post[post.size() / 2] / KV;
    };
    double v1 = vball_canal(c.r1), v2 = vball_canal(c.r2);
    if (v1 > 0 && v2 > 0) {
        double med = 0.5 * (v1 + v2);
        if (std::fabs(v1 - v2) / med > 0.15) {          // divergence > 15 % -> faux lobe, on prend la médiane
            e.vball_kmh = med;
        } else {
            e.vball_kmh = med;
        }
    } else {
        e.vball_kmh = (v1 > 0) ? v1 : v2;
    }
    e.vball_corr_kmh = e.vball_kmh / (cos(THETA_RADAR) * cos(caseRad));   // correction azimut 20° + élévation valise
    // --- 2e passe : on restreint la recherche du ridge à ±20 % autour de f_D (vitesse à priori) ---
    // L'écho de la tête de club (fréquence plus basse, très puissant) décélère après l'impact et
    // accroche sinon le ridge des 1res frames post-impact ; en bande restreinte, le ridge suit
    // la balle dès l'impact. vBall est raffiné sur cette passe, et l'appariement stéréo aussi.
    if (e.vball_kmh > 0) {
        double fD = e.vball_kmh * KV;
        double flo = std::max(1000.0, fD * 0.85), fhi = std::min(6000.0, fD * 1.10);
        spectrogram(c.r1, NFFT, HOP, ridge, power, tg, flo, fhi);
        std::vector<double> post;
        for (size_t i = i0; i < ridge.size() && tg[i] < tg[i0] + 0.05; i++) post.push_back(ridge[i]);
        if (!post.empty()) {
            std::sort(post.begin(), post.end());
            double vref = post[post.size() / 2] / KV;
            if (vref > 0) {
                e.vball_kmh = vref;
                e.vball_corr_kmh = vref / (cos(THETA_RADAR) * cos(caseRad));
            }
        }
    }
    // --- spin par périodicité du patch, avec score d'harmoniques (comme Python) ---
    int nseg = (int)(0.25 * FS);
    if ((i0 * HOP + nseg) < (int)c.r1.size() && nseg > NFFT) {
        int N = 1; while (N < nseg) N <<= 1;
        std::vector<double> seg(c.r1.begin() + i0 * HOP, c.r1.begin() + i0 * HOP + nseg);
        if (e.vball_kmh > 0) {
            // passe-bande autour du lobe balle (vitesse à priori = ridge STFT) : rejette
            // l'écho de la tête de club et le bruit hors lobe avant l'enveloppe de Hilbert
            seg = bandpass(seg, e.vball_kmh * KV, 2000.0);
        }
        std::vector<double> re(N, 0.0), im(N, 0.0);
        for (int i = 0; i < nseg; i++) re[i] = seg[i];
        fft(re, im, false);
        std::vector<double> h(N, 0.0);
        h[0] = 1.0; for (int k = 1; k < N / 2; k++) h[k] = 2.0; h[N / 2] = 1.0;
        for (int k = 0; k < N; k++) { re[k] *= h[k]; im[k] *= h[k]; }
        fft(re, im, true);                                   // signal analytique
        std::vector<double> env(N);
        for (int i = 0; i < N; i++) env[i] = sqrt(re[i] * re[i] + im[i] * im[i]);
        double m = 0; for (int i = 0; i < N; i++) m += env[i]; m /= N;
        for (int i = 0; i < N; i++) { re[i] = (env[i] - m) * (0.5 - 0.5 * cos(2.0 * M_PI * i / (N - 1))); im[i] = 0; }
        fft(re, im, false);
        std::vector<double> Pe(N / 2);
        std::vector<int> loc;
        double pmax = 0;
        for (int k = 1; k < N / 2; k++) {
            double f = k * FS / N;
            Pe[k] = re[k] * re[k] + im[k] * im[k];
            if (f >= 50.0 && f <= 200.0 && Pe[k] > 0.1 * 0 + 0) {} // bande f_rot 3000..12000 rpm
            if (f >= 50.0 && f <= 200.0 && Pe[k] > pmax) pmax = Pe[k];
            if (f >= 50.0 && f <= 200.0 && k > 0 && k < N / 2 - 1 && Pe[k] >= Pe[k-1] && Pe[k] >= Pe[k+1] && Pe[k] > 0.1 * pmax)
                loc.push_back(k);
        }
        if (loc.empty() && pmax > 0) { int k = 1; for (int j = 1; j < N / 2; j++) if (Pe[j] > Pe[k]) k = j; loc.push_back(k); }
        // score d'harmoniques : le fondamental explique le plus de pics (évite f_rot/2)
        double df = FS / N;
        int best = loc.empty() ? -1 : loc[0]; double bestscore = -1;
        for (int p : loc) {
            double f0 = p * df, score = Pe[p];
            for (int q : loc) {
                if (q == p) continue;
                int k = (int)lround(q * df / f0);
                if (k >= 1 && std::fabs(q * df - k * f0) < 2 * df) score += Pe[q];
            }
            if (score > bestscore) { bestscore = score; best = p; }
        }
        if (best < 0 && !loc.empty()) best = loc[0];              // fallback : 1er pic local
        if (best < 0) { int k = 1; for (int j = 1; j < N / 2; j++) if (Pe[j] > Pe[k]) k = j; if (Pe[k] > 0) best = k; } // fallback : max global
        if (best > 0) {
            e.spin_rpm = 60.0 * parabolic_peak(Pe, best) * df;
            // confiance = pic / médiane du spectre dans la bande f_rot 50..200 Hz
            std::vector<double> band;
            for (int k = 1; k < N / 2; k++) {
                double f = k * FS / N;
                if (f >= 50.0 && f <= 200.0) band.push_back(Pe[k]);
            }
            if (!band.empty()) {
                std::nth_element(band.begin(), band.begin() + band.size() / 2, band.end());
                double med = band[band.size() / 2];
                e.spin_patch_conf = (med > 0) ? Pe[best] / med : 0.0;
            }
        }
    }
    // --- launch angle ψ : fit du vol 2D (traînée de White + gravité + Magnus, comme flight_ode)
    // sur la fréquence instantanée f_i(t) du lobe balle (signal analytique passe-bande autour de
    // f_D, lissée sur ~1,5 période de rotation pour moyenner la modulation micro-Doppler ±β·f_rot).
    // Le radar mesure v_r(t) ≈ V(t)·cos(Φ), Φ quasi constant (cf. THETA_RADAR) : on fitte la FORME
    // de V(t) à un facteur d'échelle libre c près, v_r_model(t) = c·V_model(t; V0, ψ).
    if (e.vball_kmh > 0) {
        const double rho = 1.292 * 273.15 / 293.15;             // kg/m³ à 20 °C (comme Golfball.sci)
        const double Ab = M_PI * pow(BALL_D / 2, 2), mm = 0.04545, mu = 1.5e-5;  // mu = viscosité CINÉMATIQUE (m²/s)
        const double Cl1 = 0.54;                                 // fer 7 (SAC)
        double fD = e.vball_kmh * KV;
        int i0s = (int)(e.t0_s * FS);
        int n = std::min((int)(0.45 * FS), (int)c.r1.size() - i0s);
        if (n > 8192) {
            std::vector<double> seg(c.r1.begin() + i0s, c.r1.begin() + i0s + n);
            std::vector<double> xf = bandpass(seg, fD, 2000.0);
            int N = 1; while (N < n) N <<= 1;
            std::vector<double> re(N, 0.0), im(N, 0.0);
            for (int i = 0; i < n; i++) re[i] = xf[i];
            fft(re, im, false);
            std::vector<double> h(N, 0.0);
            h[0] = 1.0; for (int k = 1; k < N / 2; k++) h[k] = 2.0; h[N / 2] = 1.0;
            for (int k = 0; k < N; k++) { re[k] *= h[k]; im[k] *= h[k]; }
            fft(re, im, true);
            std::vector<double> fi(n - 1);
            double prev = atan2(im[0], re[0]);
            for (int i = 1; i < n; i++) {
                double ph = atan2(im[i], re[i]);
                double d = ph - prev;
                while (d > M_PI) d -= 2.0 * M_PI;
                while (d < -M_PI) d += 2.0 * M_PI;
                fi[i - 1] = d * FS / (2.0 * M_PI);
                prev = ph;
            }
            // lissage sur ~1,5 période de rotation (moyenne glissante) : enlève ±β·f_rot
            double frot = std::max(e.spin_rpm, 1200.0) / 60.0;
            int wsm = std::max(1, (int)(1.5 / frot * FS) / 2);
            // agrégation en K bins sur [0, 0.35 s]
            const int K = 12;
            double tmax = std::min(0.35, (n - 1) / FS);
            std::vector<double> tm, vrm;
            for (int b = 0; b < K; b++) {
                double ta = (b + 0.5) / K * tmax;
                int ia = (int)(ta * FS), wa = std::max(1, (int)(0.5 / K * tmax * FS));
                // bins tronqués par le lissage de bord (convolution 'same' à padding nul) : on les écarte
                if (ia - wa < wsm + 1 || ia + wa > n - 2 - wsm - 1) continue;
                double sm = 0; int c2 = 0;
                for (int j = ia - wa; j <= ia + wa; j++) {
                    double ss = 0; int c3 = 0;
                    for (int q = j - wsm; q <= j + wsm; q++) { ss += fi[q]; c3++; }
                    sm += ss / c3; c2++;
                }
                tm.push_back(ta);
                vrm.push_back((sm / c2) / KV / 3.6);
            }
            double vr0 = vrm.empty() ? 0 : vrm[0];
            if (tm.size() >= 5 && vr0 > 0) {
                double w = e.spin_rpm * 2.0 * M_PI / 60.0;           // backspin (rad/s), voie patch
                const double dt = 0.002;
                // FIT CONJOINT (ψ, ε) : grille 2D. Le radar mesure v_r = V·cos(ψ−ε)·cos θ_h.
                // ε entre par la dé-projection V0 = v_r0 / cos(ψ−ε) (qui change la traînée ∝ V0²),
                // ψ par la forme du vol (g·sinψ, etc.) : les deux sont alors identifiables.
                // --caseangle_deg fixe ε (grille 1D sur ψ seul) et rend la main à l'utilisateur.
                double bestPsi = 0, bestEps = 0, bestSse = 1e18;
                double epsStep = (caseDeg != 0.0) ? 1.0 : 1.0;
                double epsLo = (caseDeg != 0.0) ? caseDeg : 0.0;
                double epsHi = (caseDeg != 0.0) ? caseDeg : 20.0;
                for (double epsDeg = epsLo; epsDeg <= epsHi + 1e-9; epsDeg += epsStep) {
                    double eps = epsDeg * M_PI / 180.0;
                    for (double psiDeg = 4.0; psiDeg <= 26.0; psiDeg += 0.25) {
                        double psi = psiDeg * M_PI / 180.0;
                        double V0 = vr0 / std::max(cos(psi - eps), 0.3);
                        // le modèle démarre au premier bin (t = tm[0]) pour aligner donnée et modèle
                        double vx = V0 * cos(psi), vy = V0 * sin(psi), t = tm[0];
                        std::vector<double> Vm(tm.size(), 0.0);
                        for (size_t f = 0; f < tm.size(); f++) {
                            while (t < tm[f]) {
                                double V = sqrt(vx * vx + vy * vy);
                                double Re = V * BALL_D / mu;   // mu cinématique : Re = V·d/ν (comme flightg Scilab et EquationODEFlight)
                                double Cd = 0.36 + 24.0 / Re + 6.0 / (1.0 + sqrt(Re));   // White
                                double Cm = Cl1 * pow(BALL_D / 2, 2) * pow(w / std::max(V, 1e-6), 0.4);
                                double FD = 0.5 * rho * Cd * Ab * V * V;               // traînée ∝ V²
                                // Magnus comme flight_ode (Python) : F_M = 0.5·ρ·Cm·A·V·(ω×v), |ω×v| = ω·V
                                double kM = 0.5 * rho * Cm * Ab * V * w;
                                // accélération = −g + (forces aéro)/m  (g n'est PAS divisé par m)
                                vx += (-FD * vx / V - kM * vy) / mm * dt;
                                vy += (-9.81 + (-FD * vy / V + kM * vx) / mm) * dt;
                                t += dt;
                            }
                            Vm[f] = sqrt(vx * vx + vy * vy);
                        }
                        // échelle libre c sur v_r_model = c · V_model · cos(ψ−ε)
                        double num = 0, den = 0;
                        for (size_t f = 0; f < tm.size(); f++) { double m = Vm[f] * cos(psi - eps); num += vrm[f] * m; den += m * m; }
                        double c = (den > 0) ? num / den : 0;
                        double sse = 0;
                        for (size_t f = 0; f < tm.size(); f++) { double m = Vm[f] * cos(psi - eps); double d = vrm[f] - c * m; sse += d * d; }
                        if (sse < bestSse) { bestSse = sse; bestPsi = psiDeg; bestEps = epsDeg; }
                    }
                }
                double mean = 0; for (double v : vrm) mean += v; mean /= vrm.size();
                double sst = 0; for (double v : vrm) { double d = v - mean; sst += d * d; }
                e.launch_deg = bestPsi;              // vrai ψ (ε séparé par le fit conjoint)
                if (e.launch_deg > 45.0) e.launch_deg = 45.0;
                if (caseDeg == 0.0) e.case_deg = bestEps;   // ε mesuré par le fit (0 si --caseangle_deg forcé)
                e.launch_conf = (sst > 0) ? std::max(0.0, std::min(1.0, 1.0 - bestSse / sst)) : 0.0;
                // correction fine de vBall avec ψ et ε : cos γ = cos ε·cos(20°)·cos ψ + sin ε·sin ψ
                if (e.case_deg != 0.0 && e.launch_deg > 0) {
                    double psi = e.launch_deg * M_PI / 180.0, er = e.case_deg * M_PI / 180.0;
                    double cg = cos(er) * cos(THETA_RADAR) * cos(psi) + sin(er) * sin(psi);
                    if (cg > 0.3) e.vball_corr_kmh = e.vball_kmh / cg;
                }
            }
        }
    }
    // --- alpha(t) par appariement stéréo : f2 cherché au voisinage de f1 (±300 Hz), |f1-f2|<=30 Hz ---
    // spectrogramme du canal 2 sur la même bande restreinte autour de f_D (rejette l'écho club)
    std::vector<double> ridge2, power2, tg2;
    if (e.vball_kmh > 0) {
        double fD = e.vball_kmh * KV;
        spectrogram(c.r2, NFFT, HOP, ridge2, power2, tg2,
                    std::max(1000.0, fD * 0.85), std::min(6000.0, fD * 1.10));
    } else {
        spectrogram(c.r2, NFFT, HOP, ridge2, power2, tg2);
    }
    std::vector<double> tt, al;
    double v_ms = e.vball_kmh / KV * 1000.0 / 3600.0;
    for (size_t i = i0; i < ridge.size(); i++) {
        if (power[i] < 0.02 * power[i0]) continue;
        double f1 = ridge[i];
        // chercher le pic de R2 dans ±300 Hz autour de f1
        int nframes2 = (int)ridge2.size();
        if ((int)i >= nframes2) continue;
        // spectre de la frame i du canal 2
        std::vector<double> re(NFFT), im(NFFT);
        std::vector<double> w(NFFT);
        for (int j = 0; j < NFFT; j++) w[j] = 0.5 - 0.5 * cos(2.0 * M_PI * j / (NFFT - 1));
        for (int j = 0; j < NFFT; j++) { re[j] = c.r2[i * HOP + j] * w[j]; im[j] = 0; }
        fft(re, im, false);
        std::vector<double> P2(NFFT / 2);
        for (int k = 1; k < NFFT / 2; k++) P2[k] = re[k] * re[k] + im[k] * im[k];
        int kbest = -1; double pbest = 0;
        for (int k = 1; k < NFFT / 2; k++) {
            double f = k * FS / NFFT;
            if (f < f1 - 300 || f > f1 + 300) continue;
            if (P2[k] > pbest) { pbest = P2[k]; kbest = k; }
        }
        if (kbest < 0 || pbest < 0.01 * power2[i0]) continue;
        double f2 = parabolic_peak(P2, kbest) * FS / NFFT;
        if (std::fabs(f1 - f2) > 30.0) continue;              // cohérence géométrique
        double fc = (f1 + f2) / 2.0, fd = (f1 - f2) / 2.0;
        if (fc <= 0) continue;
        double L = L0 + v_ms / cos(THETA_RADAR) * (tg[i] - tg[i0]);  // L(t) évolutif
        tt.push_back(tg[i]);
        al.push_back(atan((2.0 * L / B_BASE) * (fd / fc)) * 180.0 / M_PI);
    }
    fill_spec(e, c.r1, 512, 128);   // fenêtre 11,6 ms comme plot_spectro.py (ridge bien résolu en temps)
    if (al.size() >= 3) {
        // alpha0 = médiane fenêtre initiale 50 ms ; dalpha = médiane(fin 100 ms) - médiane(début)
        std::vector<double> a0, af;
        for (size_t i = 0; i < tt.size(); i++) {
            if (tt[i] <= tg[i0] + 0.05) a0.push_back(al[i]);
            if (tt[i] >= tt.back() - 0.10) af.push_back(al[i]);
        }
        if (!a0.empty() && !af.empty()) {
            std::sort(a0.begin(), a0.end()); std::sort(af.begin(), af.end());
            e.alpha0_deg = a0[a0.size() / 2];
            e.dalpha_deg = af[af.size() / 2] - a0[a0.size() / 2];
        }
    }
    return e;
}


// ---------------- vitesse du club à l'impact ---------------------------------
// Le lobe Doppler de la tête de club juste avant l'impact (t < t0) contient vClub.
// 1) estimation modèle inverse : vguess = vBall / 1.35 (smash typique)
// 2) filtrage passe-bande du signal autour de fguess = vguess * KV (±15 %)
// 3) crête du spectrogramme dans cette bande sur [t0-0.30 s, t0] -> vClub à l'impact
// Retourne NaN si le lobe club n'est pas détectable.
CAP_UNUSED static double estimate_club_speed(const Capture& c, const EstRadar& e) {
    if (e.vball_kmh <= 0) return NAN;
    double vguess = e.vball_kmh / 1.35;
    double fc = vguess * KV;
    double bw = std::max(200.0, 0.30 * fc);            // ±15 % autour de l'estimation
    const int NFFT = 1024, HOP = 128;
    int i0 = (int)(e.t0_s * FS);
    int n0 = std::max(0, i0 - (int)(0.30 * FS));
    int n1 = std::min((int)c.r1.size(), i0 + NFFT);
    if (n1 - n0 < 2 * NFFT) return NAN;
    // passe-bande FFT sur la fenêtre [n0, n1)
    int n = n1 - n0;
    int N = 1; while (N < n) N <<= 1;
    std::vector<double> re(N, 0.0), im(N, 0.0);
    double m = 0; for (int i = 0; i < n; i++) m += c.r1[n0 + i]; m /= n;
    for (int i = 0; i < n; i++) re[i] = c.r1[n0 + i] - m;
    fft(re, im, false);
    for (int k = 0; k < N; k++) {
        double f = (k <= N / 2) ? k * FS / N : (k - N) * FS / N;
        if (std::fabs(f - fc) > bw / 2.0) { re[k] = 0.0; im[k] = 0.0; }
    }
    fft(re, im, true);
    std::vector<double> x(n);
    for (int i = 0; i < n; i++) x[i] = re[i];
    // spectrogramme, pic restreint à la bande [fc-bw/2, fc+bw/2]
    int nframes = 1 + (n - NFFT) / HOP;
    if (nframes < 4) return NAN;
    std::vector<double> w(NFFT);
    for (int i = 0; i < NFFT; i++) w[i] = 0.5 - 0.5 * cos(2.0 * M_PI * i / (NFFT - 1));
    int k_lo = std::max(1, (int)((fc - bw / 2.0) * NFFT / FS));
    int k_hi = std::min(NFFT / 2 - 1, (int)((fc + bw / 2.0) * NFFT / FS) + 1);
    std::vector<double> vals, pw, P(NFFT / 2), re2(NFFT), im2(NFFT);
    double pmax_all = 0;
    for (int fr = 0; fr < nframes; fr++) {
        for (int i = 0; i < NFFT; i++) { re2[i] = x[fr * HOP + i] * w[i]; im2[i] = 0; }
        fft(re2, im2, false);
        for (int k = 0; k < NFFT / 2; k++) P[k] = re2[k] * re2[k] + im2[k] * im2[k];
        int kb = k_lo; double pb = 0;
        for (int k = k_lo; k <= k_hi; k++) if (P[k] > pb) { pb = P[k]; kb = k; }
        if (pb > pmax_all) pmax_all = pb;
        vals.push_back(parabolic_peak(P, kb) * FS / NFFT);
        pw.push_back(pb);
    }
    // frames où le lobe club est présent (puissance significative)
    std::vector<double> good;
    for (size_t i = 0; i < vals.size(); i++) if (pw[i] > 0.05 * pmax_all) good.push_back(vals[i] / KV);
    if (good.size() < 3) return NAN;
    std::sort(good.begin(), good.end());
    return good[(size_t)(good.size() * 0.9)];   // quantile 90 % ~ vitesse juste avant l'impact
}


// ---------------- spin par peigne de raies micro-Doppler ---------------------
// Spectre de puissance moyen (Welch, NFFT 8192) post-impact dans la bande
// f_D ± 2 kHz, puis AUTOCORRÉLATION en fréquence : le pic au lag f_rot donne
// le spin. Robuste même quand les raies individuelles sont sous le fond (β petit),
// car tout le peigne contribue en cohérence. Retourne 0 si rien trouvé.
// même analyse, en exposant l'autocorrélation (pour affichage) : lag en Hz, ac normalisée par ac[0]
CAP_UNUSED static double estimate_spin_peigne_ac(const Capture& c, const EstRadar& e,
                                                 std::vector<double>* lagHz, std::vector<double>* acv,
                                                 double* conf = nullptr) {
    if (e.vball_kmh <= 0) return 0.0;
    double fD = e.vball_kmh * KV;
    int i0 = (int)(e.t0_s * FS);
    int n1 = std::min((int)c.r1.size(), i0 + (int)(0.5 * FS));
    if (n1 - i0 < 8192) return 0.0;
    const int NFFT = 8192, HOP = 4096;
    int k0 = std::max(1, (int)((fD - 2000.0) * NFFT / FS));
    int k1 = std::min(NFFT / 2 - 1, (int)((fD + 2000.0) * NFFT / FS));
    int nb = k1 - k0 + 1;
    if (nb < 100) return 0.0;
    std::vector<double> acc(nb, 0.0), w(NFFT), re(NFFT), im(NFFT);
    for (int i = 0; i < NFFT; i++) w[i] = 0.5 - 0.5 * cos(2.0 * M_PI * i / (NFFT - 1));
    int cnt = 0;
    for (int st = i0; st + NFFT <= n1; st += HOP) {
        for (int i = 0; i < NFFT; i++) { re[i] = c.r1[st + i] * w[i]; im[i] = 0; }
        fft(re, im, false);
        for (int k = k0; k <= k1; k++) acc[k - k0] += re[k] * re[k] + im[k] * im[k];
        cnt++;
    }
    if (cnt == 0) return 0.0;
    for (int i = 0; i < nb; i++) acc[i] /= cnt;
    // detrend par moyenne glissante (~200 Hz) : enlève le lobe porteuse large
    int win = std::max(3, (int)(200.0 / (FS / NFFT)) / 2);
    std::vector<double> tr(nb);
    for (int i = 0; i < nb; i++) {
        double s = 0; int c = 0;
        for (int j = std::max(0, i - win); j <= std::min(nb - 1, i + win); j++) { s += acc[j]; c++; }
        tr[i] = acc[i] - s / c;
    }
    double df = FS / NFFT;
    int lag0 = std::max(2, (int)(45.0 / df));           // 2700 rpm min (au-delà du lobe porteuse)
    int lag1 = std::min(nb - 2, (int)(200.0 / df));      // 12000 rpm max
    if (lag1 <= lag0 + 2) return 0.0;
    std::vector<double> ac(lag1 + 1, 0.0);
    for (int lag = 0; lag <= lag1; lag++) {
        double s = 0;
        for (int i = 0; i + lag < nb; i++) s += tr[i] * tr[i + lag];
        ac[lag] = s / (nb - lag);                        // normalisée (sinon pic artificiel à lag~0)
    }
    // score de fondamental : le lag f_rot explique aussi ses harmoniques k*f_rot
    int best = lag0; double bscore = -1e18;
    for (int lag = lag0; lag <= lag1; lag++) {
        double score = 0;
        for (int k = 1; k <= 4 && k * lag <= lag1; k++) score += ac[k * lag] / k;
        if (score > bscore) { bscore = score; best = lag; }
    }
    if (bscore <= 0) return 0.0;
    if (conf) {
        // confiance = pic fondamental / médiane de l'autocorrélation sur la plage cherchée
        std::vector<double> base(ac.begin() + lag0, ac.begin() + lag1 + 1);
        std::nth_element(base.begin(), base.begin() + base.size() / 2, base.end());
        double med = base[base.size() / 2];
        *conf = (med > 0) ? ac[best] / med : 0.0;
    }
    double lagf = parabolic_peak(ac, best);
    if (lagHz && acv && ac[0] > 0) {
        for (int lag = 0; lag <= lag1; lag++) { lagHz->push_back(lag * df); acv->push_back(ac[lag] / ac[0]); }
    }
    return 60.0 * lagf * df;                            // f_rot (Hz) -> rpm
}
CAP_UNUSED static double estimate_spin_peigne(const Capture& c, const EstRadar& e) {
    return estimate_spin_peigne_ac(c, e, nullptr, nullptr);
}

// ---------------- passe-bande FFT : garde |f-f0| <= bw/2 (définition) ----------------
CAP_UNUSED static std::vector<double> bandpass(const std::vector<double>& x, double f0, double bw) {
    int n = (int)x.size();
    int N = 1; while (N < n) N <<= 1;
    std::vector<double> re(N, 0.0), im(N, 0.0);
    double m = 0; for (int i = 0; i < n; i++) m += x[i]; m /= n;
    for (int i = 0; i < n; i++) re[i] = x[i] - m;
    fft(re, im, false);
    for (int k = 0; k < N; k++) {
        double f = (k <= N / 2) ? k * FS / N : (k - N) * FS / N;
        if (std::fabs(f - f0) > bw / 2.0) { re[k] = 0.0; im[k] = 0.0; }
    }
    fft(re, im, true);
    std::vector<double> y(n);
    for (int i = 0; i < n; i++) y[i] = re[i];
    return y;
}

// ---------------- spin par fréquence instantanée (phase) ---------------------
// Le lobe balle filtré passe-bande autour de f_D, mis en signal analytique (Hilbert),
// a pour phase φ(t) = 2π f_D t + β sin(2π f_rot t). La fréquence instantanée
// dφ/dt - tendance = β f_rot cos(2π f_rot t) : le pic de son spectre à f_rot donne
// le spin — plus propre que l'enveloppe d'amplitude (patch).
// même analyse, en exposant le spectre de la fréquence instantanée detrendée (40..200 Hz, dB)
CAP_UNUSED static double estimate_spin_phase_spec(const Capture& c, const EstRadar& e,
                                                  std::vector<double>* sf, std::vector<double>* sdb,
                                                  double* exc_rpm = nullptr, double* exc_conf = nullptr,
                                                  double* conf = nullptr) {
    if (e.vball_kmh <= 0) return 0.0;
    double fD = e.vball_kmh * KV;
    int i0 = (int)(e.t0_s * FS);
    int n = std::min((int)(0.5 * FS), (int)c.r1.size() - i0);
    if (n < 8192) return 0.0;
    std::vector<double> seg(c.r1.begin() + i0, c.r1.begin() + i0 + n);
    std::vector<double> xf = bandpass(seg, fD, 2000.0);
    // signal analytique
    int N = 1; while (N < n) N <<= 1;
    std::vector<double> re(N, 0.0), im(N, 0.0);
    for (int i = 0; i < n; i++) re[i] = xf[i];
    fft(re, im, false);
    std::vector<double> h(N, 0.0);
    h[0] = 1.0; for (int k = 1; k < N / 2; k++) h[k] = 2.0; h[N / 2] = 1.0;
    for (int k = 0; k < N; k++) { re[k] *= h[k]; im[k] *= h[k]; }
    fft(re, im, true);
    // fréquence instantanée (dérivée de phase unwrappée)
    std::vector<double> fi(n - 1);
    double prev = atan2(im[0], re[0]);
    for (int i = 1; i < n; i++) {
        double ph = atan2(im[i], re[i]);
        double d = ph - prev;
        while (d > M_PI) d -= 2.0 * M_PI;
        while (d < -M_PI) d += 2.0 * M_PI;
        fi[i - 1] = d * FS / (2.0 * M_PI);
        prev = ph;
    }
    // excursion micro-Doppler (voie β) : sur fi juste après l'impact (fenêtre courte, la dérive
    // Doppler y est faible), detrend LINEAIRE (la moyenne glissante 0,1 s ci-dessous atténuerait
    // l'oscillation à f_rot), puis percentiles 2,5–97,5 -> Δf crête-crête robuste.
    // s(t) = cos(2π f_D t + β sin ωt), β = 4πr/λ = 60·KS  =>  Δf = 2β·f_rot = 2·KS·rpm.
    if (exc_rpm) {
        int m0 = std::min((int)(0.30 * FS), (int)fi.size());   // ~300 ms post-impact
        if (m0 > 1024) {
            // lissage de fi (moyenne glissante ~2 ms) : le bruit de phase par échantillon
            // (~±350 Hz) dominerait sinon l'excursion micro-Doppler
            int w2 = std::max(1, (int)(0.002 * FS) / 2);
            std::vector<double> y(m0);
            for (int i = 0; i < m0; i++) {
                double s = 0; int cc = 0;
                for (int j = std::max(0, i - w2); j <= std::min(m0 - 1, i + w2); j++) { s += fi[j]; cc++; }
                y[i] = s / cc;
            }
            // detrend linéaire (moindres carrés) : enlève la dérive Doppler
            double sx = 0, sy = 0, sxx = 0, sxy = 0;
            for (int i = 0; i < m0; i++) { sx += i; sy += y[i]; sxx += (double)i * i; sxy += (double)i * y[i]; }
            double den = m0 * sxx - sx * sx;
            double b = (den != 0) ? (m0 * sxy - sx * sy) / den : 0.0;
            double a = (sy - b * sx) / m0;
            std::vector<double> s(m0);
            for (int i = 0; i < m0; i++) s[i] = y[i] - (a + b * i);
            std::sort(s.begin(), s.end());
            double lo = s[(size_t)(0.025 * (m0 - 1))], hi = s[(size_t)(0.975 * (m0 - 1))];
            *exc_rpm = (hi - lo) / (2.0 * KS);   // Δf [Hz] / (2·KS [Hz/rpm]) -> rpm
            if (exc_conf) {
                // confiance = excursion / écart-type robuste (1.4826·MAD) du résidu (s déjà trié)
                double med = s[m0 / 2];
                std::vector<double> absd(m0);
                for (int i = 0; i < m0; i++) absd[i] = std::fabs(s[i] - med);
                std::nth_element(absd.begin(), absd.begin() + m0 / 2, absd.end());
                double sig = 1.4826 * absd[m0 / 2];
                *exc_conf = (sig > 0) ? (hi - lo) / sig : 0.0;
            }
        } else {
            *exc_rpm = 0.0;
        }
    }
    // detrend : moyenne glissante 0,1 s (enlève la décélération Doppler f_D(t))
    int win = (int)(0.1 * FS) / 2;
    int m = (int)fi.size();
    std::vector<double> tr(m);
    for (int i = 0; i < m; i++) {
        double s = 0; int cc = 0;
        for (int j = std::max(0, i - win); j <= std::min(m - 1, i + win); j++) { s += fi[j]; cc++; }
        tr[i] = fi[i] - s / cc;
    }
    // spectre de tr, fenêtre Hann, pic 40..200 Hz avec score d'harmoniques
    int N2 = 1; while (N2 < m) N2 <<= 1;
    std::vector<double> re2(N2, 0.0), im2(N2, 0.0);
    for (int i = 0; i < m; i++) re2[i] = tr[i] * (0.5 - 0.5 * cos(2.0 * M_PI * i / (m - 1)));
    fft(re2, im2, false);
    std::vector<double> P(N2 / 2);
    for (int k = 0; k < N2 / 2; k++) P[k] = re2[k] * re2[k] + im2[k] * im2[k];
    double df = FS / N2;
    int k0 = std::max(1, (int)(40.0 / df)), k1 = std::min(N2 / 2 - 1, (int)(200.0 / df));
    if (k1 <= k0 + 2) return 0.0;
    std::vector<int> loc;
    double pmax = 0;
    for (int k = k0; k <= k1; k++) if (P[k] > pmax) pmax = P[k];
    if (pmax <= 0) return 0.0;
    for (int k = k0 + 1; k < k1; k++)
        if (P[k] >= P[k - 1] && P[k] >= P[k + 1] && P[k] > 0.2 * pmax) loc.push_back(k);
    if (loc.empty()) { int k = k0; for (int j = k0; j <= k1; j++) if (P[j] > P[k]) k = j; loc.push_back(k); }
    int best = loc[0]; double bs = -1;
    for (int p : loc) {
        double f0 = p * df, score = P[p];
        for (int q : loc) { int k = (int)lround(q * df / f0); if (k >= 1 && std::fabs(q * df - k * f0) < 2 * df) score += P[q]; }
        if (score > bs) { bs = score; best = p; }
    }
    if (sf && sdb && pmax > 0) {
        for (int k = k0; k <= k1; k++) {
            sf->push_back(k * df);
            double d = 10.0 * log10(P[k] / pmax + 1e-12);
            sdb->push_back(d < -80.0 ? -80.0 : d);
        }
    }
    if (conf) {
        // confiance = pic / médiane du spectre de fi detrendée (bande 40..200 Hz)
        std::vector<double> base(P.begin() + k0, P.begin() + k1 + 1);
        std::nth_element(base.begin(), base.begin() + base.size() / 2, base.end());
        double med = base[base.size() / 2];
        *conf = (med > 0) ? P[best] / med : 0.0;
    }
    return 60.0 * parabolic_peak(P, best) * df;
}
CAP_UNUSED static double estimate_spin_phase(const Capture& c, const EstRadar& e) {
    return estimate_spin_phase_spec(c, e, nullptr, nullptr);
}

// ---------------- fusion patch + phase + peigne + excursion β, pondérée par confiance ----------------
// Chaque voie a une confiance (pic / fond spectral, ex. pic/médiane). La référence est la voie
// valide (rpm > 0, conf > 0) de plus grande confiance ; on moyenne alors toutes les voies
// cohérentes à ±15 % de la référence, pondérées par leur confiance ; sinon on garde la
// référence seule. Sans aucune confiance disponible, repli sur la 1re voie rpm > 0.
CAP_UNUSED static double fusion_spin(double patch_rpm, double patch_conf,
                                     double phase_rpm, double phase_conf,
                                     double peigne_rpm, double peigne_conf,
                                     double exc_rpm, double exc_conf) {
    const double v[4] = {patch_rpm, phase_rpm, peigne_rpm, exc_rpm};
    // la voie excursion a un biais SYSTEMATIQUE connu (β effectif < 4πr/λ sur balle réelle :
    // elle lit bas) : sa confiance SNR est rabattue pour ne pas prendre la main seule.
    const double EXC_CONF_DISCOUNT = 0.25;
    const double c[4] = {patch_conf, phase_conf, peigne_conf, exc_conf * EXC_CONF_DISCOUNT};
    int ref = -1;
    for (int i = 0; i < 4; i++)
        if (v[i] > 0 && c[i] > 0 && (ref < 0 || c[i] > c[ref])) ref = i;
    if (ref < 0) {
        for (int i = 0; i < 4; i++)
            if (v[i] > 0) { ref = i; break; }
        return (ref < 0) ? 0.0 : v[ref];
    }
    double num = 0, den = 0;
    for (int i = 0; i < 4; i++) {
        if (v[i] <= 0) continue;
        if (std::fabs(v[i] - v[ref]) / std::max(v[i], v[ref]) < 0.15) {
            double w = (c[i] > 0) ? c[i] : 1.0;
            num += w * v[i];
            den += w;
        }
    }
    return (den > 0) ? num / den : v[ref];
}


// signal Doppler synthetique a partir d'une trajectoire simulee (mode club, pour le spectrogramme en bas)
CAP_UNUSED static std::vector<double> gen_signal_simu(const std::vector< std::vector<double> >& mf, double vclub_kmh, double fs = FS) {
    const double lam = 299792458.0 / 10.525e9;
    const double r = BALL_D / 2.0;
    int N = (int)(mf.size() * 0.01 * fs) + (int)(0.15 * fs);
    std::vector<double> s(N, 0.0);
    double phase = 0.0;
    int t0i = (int)(0.10 * fs);
    double spin = 6000.0 * 2.0 * M_PI / 60.0;
    for (int n = 0; n < N; n++) {
        double t = n / fs;
        double fD;
        if (n < t0i) {
            double vc = vclub_kmh / 3.6 * pow((double)n / t0i, 2.5);
            fD = KV * vc * 3.6;
        } else {
            int k = (int)((t - 0.10) / 0.01);
            if (k >= (int)mf.size()) k = (int)mf.size() - 1;
            if (k < 0) k = 0;
            double V = sqrt(mf[k][1]*mf[k][1] + mf[k][3]*mf[k][3] + mf[k][5]*mf[k][5]);
            fD = KV * V * 3.6 * cos(2.0 * M_PI / 180.0);
        }
        phase += 2.0 * M_PI * fD / fs;
        double patch = 0.3 * sin((2.0 * r / lam) * sin(spin * t));
        double sig = cos(phase) * (1.0 + patch);
        if (n > t0i - 200 && n < t0i + 200) sig += 3.0 * exp(-pow((n - t0i) / 60.0, 2.0)) * cos(2.0 * M_PI * 2200.0 * t);
        s[n] = 0.4 * sig + 0.02 * (rand() / (double)RAND_MAX - 0.5) * 2.0;
    }
    return s;
}

#endif // CAPTURE_H_

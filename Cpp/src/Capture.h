//============================================================================
// Capture.h — lecture captation radar (CSV 2 lignes ';' ou WAV stéréo) et
// estimation simplifiée : vBall (ridge STFT post-impact), spin total
// (périodicité du patch via enveloppe de Hilbert), alpha0 (appariement stéréo).
// Header-only, C++11, aucune dépendance externe (FFT radix-2 itérative).
//============================================================================
#ifndef CAPTURE_H_
#define CAPTURE_H_

#include <vector>
#include <string>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static const double KV = 19.49;        // Hz / (km/h) radar 10.525 GHz
static const double FS = 44100.0;
static const double B_BASE = 0.30;     // baseline radars (m)
static const double L0 = 1.50;         // distance initiale balle (m)
static const double THETA_RADAR = 20.0 * M_PI / 180.0;

// ---------------- FFT itérative radix-2 (longueur = puissance de 2) --------
static void fft(std::vector<double>& re, std::vector<double>& im, bool inverse) {
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

// ---------------- chargement ------------------------------------------------
struct Capture {
    std::vector<double> r1, r2;   // canaux R1/R2
    double fs = FS;
};

static Capture load_capture(const std::string& path) {
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

// ---------------- spectrogramme + ridge --------------------------------------
static void spectrogram(const std::vector<double>& x, int nfft, int hop,
                        std::vector<double>& ridge, std::vector<double>& power,
                        std::vector<double>& tgrid) {
    int n = (int)x.size();
    int nframes = 1 + (n - nfft) / hop;
    std::vector<double> w(nfft);
    for (int i = 0; i < nfft; i++) w[i] = 0.5 - 0.5 * cos(2.0 * M_PI * i / (nfft - 1)); // Hann
    ridge.assign(nframes, 0.0); power.assign(nframes, 0.0); tgrid.assign(nframes, 0.0);
    std::vector<double> re(nfft), im(nfft);
    for (int fr = 0; fr < nframes; fr++) {
        for (int i = 0; i < nfft; i++) { re[i] = x[fr * hop + i] * w[i]; im[i] = 0.0; }
        fft(re, im, false);
        int kmax = 1; double pmax = 0.0;
        for (int k = 1; k < nfft / 2; k++) {
            double f = k * FS / nfft;
            if (f < 1000.0 || f > 6000.0) continue;   // passe-bande vitesse balle
            double p = re[k] * re[k] + im[k] * im[k];
            if (p > pmax) { pmax = p; kmax = k; }
        }
        ridge[fr] = kmax * FS / nfft;
        power[fr] = pmax;
        tgrid[fr] = (fr * hop + nfft / 2.0) / FS;
    }
}

// ---------------- estimation ------------------------------------------------
struct EstRadar {
    double vball_kmh;    // radiale
    double spin_rpm;
    double alpha0_deg;
    double t0_s;
};

static EstRadar estimate(const Capture& c) {
    EstRadar e; e.vball_kmh = 0; e.spin_rpm = 0; e.alpha0_deg = 0; e.t0_s = 0;
    const int NFFT = 2048, HOP = 1024;
    std::vector<double> ridge, power, tg;
    spectrogram(c.r1, NFFT, HOP, ridge, power, tg);
    // impact = frame max énergie
    int i0 = 0; for (size_t i = 1; i < power.size(); i++) if (power[i] > power[i0]) i0 = (int)i;
    e.t0_s = tg[i0];
    // vBall = médiane du ridge post-impact (50 ms)
    std::vector<double> post;
    for (size_t i = i0; i < ridge.size() && tg[i] < tg[i0] + 0.05; i++) post.push_back(ridge[i]);
    if (!post.empty()) {
        std::sort(post.begin(), post.end());
        e.vball_kmh = post[post.size() / 2] / KV;
    }
    // spin par périodicité du patch : enveloppe de Hilbert 250 ms post-impact
    int nseg = (int)(0.25 * FS);
    if ((i0 * HOP + nseg) < (int)c.r1.size() && nseg > NFFT) {
        int N = 1; while (N < nseg) N <<= 1;
        std::vector<double> re(N, 0.0), im(N, 0.0);
        for (int i = 0; i < nseg; i++) re[i] = c.r1[i0 * HOP + i];
        fft(re, im, false);
        // signal analytique : h[0]=1, h[1..N/2-1]=2, h[N/2]=1
        std::vector<double> h(N, 0.0);
        h[0] = 1.0; for (int k = 1; k < N / 2; k++) h[k] = 2.0; h[N / 2] = 1.0;
        for (int k = 0; k < N; k++) { re[k] *= h[k]; im[k] *= h[k]; }
        fft(re, im, true);
        // enveloppe
        std::vector<double> env(N);
        for (int i = 0; i < N; i++) env[i] = sqrt(re[i] * re[i] + im[i] * im[i]);
        double m = 0; for (int i = 0; i < N; i++) m += env[i]; m /= N;
        for (int i = 0; i < N; i++) { re[i] = (env[i] - m); im[i] = 0; re[i] *= 0.5 - 0.5 * cos(2.0 * M_PI * i / (N - 1)); }
        fft(re, im, false);
        // pic spectre enveloppe dans 50..200 Hz (3000..12000 rpm)
        int kbest = -1; double pbest = 0;
        for (int k = 1; k < N / 2; k++) {
            double f = k * FS / N;
            if (f < 50.0 || f > 200.0) continue;
            double p = re[k] * re[k] + im[k] * im[k];
            if (p > pbest) { pbest = p; kbest = k; }
        }
        if (kbest > 0) e.spin_rpm = 60.0 * kbest * FS / N;
    }
    // alpha0 : pics moyens post-impact des 2 canaux (moyenne des spectres des frames post-impact)
    auto mean_peak = [&](const std::vector<double>& x) -> double {
        int nframes = 1 + ((int)x.size() - NFFT) / HOP;
        std::vector<double> acc(NFFT / 2, 0.0);
        std::vector<double> w(NFFT), re(NFFT), im(NFFT);
        for (int i = 0; i < NFFT; i++) w[i] = 0.5 - 0.5 * cos(2.0 * M_PI * i / (NFFT - 1));
        int cnt = 0;
        for (int fr = i0; fr < nframes && cnt < 20; fr++, cnt++) {
            for (int i = 0; i < NFFT; i++) { re[i] = x[fr * HOP + i] * w[i]; im[i] = 0; }
            fft(re, im, false);
            for (int k = 1; k < NFFT / 2; k++) acc[k] += re[k] * re[k] + im[k] * im[k];
        }
        int kbest = 1; double pbest = 0;
        for (int k = 1; k < NFFT / 2; k++) {
            double f = k * FS / NFFT;
            if (f < 1000.0 || f > 6000.0) continue;
            if (acc[k] > pbest) { pbest = acc[k]; kbest = k; }
        }
        return kbest * FS / NFFT;
    };
    double f1 = mean_peak(c.r1), f2 = mean_peak(c.r2);
    double fc = (f1 + f2) / 2.0, fd = (f1 - f2) / 2.0;
    if (fc > 0) e.alpha0_deg = atan((2.0 * L0 / B_BASE) * (fd / fc)) * 180.0 / M_PI;
    return e;
}

#endif // CAPTURE_H_

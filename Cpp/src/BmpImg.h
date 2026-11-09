//============================================================================
// BmpImg.h — image RGB 24 bits + mini fonte 5x7 + writers BMP et PNG
// (PNG = zlib en blocs "stored" + CRC32/Adler32 maison), zéro dépendance.
// Utilisé par spectro_png.cpp (CImg se comportait bizarrement sur le rendu pixel).
//============================================================================
#ifndef BMPIMG_H_
#define BMPIMG_H_

#include <vector>
#include <string>
#include <cstdint>
#include <cstdio>
#include <cstring>

struct BmpImg {
    int W, H;
    std::vector<uint8_t> px;  // RGB, ligne par ligne, haut -> bas
    BmpImg(int w, int h, uint8_t r = 255, uint8_t g = 255, uint8_t b = 255)
        : W(w), H(h), px((size_t)w * h * 3) {
        for (size_t i = 0; i + 2 < px.size(); i += 3) { px[i] = r; px[i+1] = g; px[i+2] = b; }
    }
    void set(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
        if (x < 0 || x >= W || y < 0 || y >= H) return;
        size_t i = ((size_t)y * W + x) * 3;
        px[i] = r; px[i+1] = g; px[i+2] = b;
    }
    void rect(int x0, int y0, int x1, int y1, uint8_t r, uint8_t g, uint8_t b, bool fill = false) {
        for (int y = y0; y <= y1; y++) for (int x = x0; x <= x1; x++) {
            if (fill || x == x0 || x == x1 || y == y0 || y == y1) set(x, y, r, g, b);
        }
    }
    void line(int x0, int y0, int x1, int y1, uint8_t r, uint8_t g, uint8_t b) {
        int dx = abs(x1 - x0), dy = -abs(y1 - y0), sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1, e = dx + dy;
        for (;;) { set(x0, y0, r, g, b); if (x0 == x1 && y0 == y1) break; int e2 = 2*e; if (e2 >= dy) { e += dy; x0 += sx; } if (e2 <= dx) { e += dx; y0 += sy; } }
    }
    void circle(int cx, int cy, int rad, uint8_t r, uint8_t g, uint8_t b) {
        for (int y = -rad; y <= rad; y++) for (int x = -rad; x <= rad; x++)
            if (x*x + y*y <= rad*rad) set(cx + x, cy + y, r, g, b);
    }
    // mini fonte 5x7 (chiffres, lettres maj, quelques signes)
    static const uint8_t* glyph(char c) {
        static const uint8_t G[96][7] = {
        //  ' ' 0x20
        {0,0,0,0,0,0,0},
        // chiffres 0-9 0x30..0x39
        {0x0E,0x11,0x13,0x15,0x19,0x11,0x0E},{0x04,0x0C,0x04,0x04,0x04,0x04,0x0E},{0x0E,0x11,0x01,0x06,0x08,0x10,0x1F},{0x1F,0x02,0x04,0x02,0x01,0x11,0x0E},{0x02,0x06,0x0A,0x12,0x1F,0x02,0x02},{0x1F,0x10,0x1E,0x01,0x01,0x11,0x0E},{0x06,0x08,0x10,0x1E,0x11,0x11,0x0E},{0x1F,0x01,0x02,0x04,0x08,0x08,0x08},{0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E},{0x0E,0x11,0x11,0x0F,0x01,0x02,0x0C},
        // : ; < = > ?  0x3A..
        {0,0,0x0C,0x0C,0,0x0C,0},{0,0,0x0C,0x0C,0,0,0},{0x02,0x04,0x08,0x10,0x08,0x04,0x02},{0,0,0x1F,0,0x1F,0,0},{0x08,0x04,0x02,0x01,0x02,0x04,0x08},{0x1F,0x08,0x04,0x02,0x04,0x08,0x1F},{0x1F,0x02,0x04,0x08,0x04,0x02,0x1F},
        // A-Z 0x41..
        {0x0E,0x11,0x11,0x1F,0x11,0x11,0x11},{0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E},{0x0E,0x11,0x10,0x10,0x10,0x11,0x0E},{0x1C,0x11,0x11,0x11,0x11,0x11,0x1C},{0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F},{0x1F,0x10,0x10,0x1E,0x10,0x10,0x10},{0x0E,0x11,0x10,0x17,0x11,0x11,0x0F},{0x11,0x11,0x11,0x1F,0x11,0x11,0x11},{0x0E,0x04,0x04,0x04,0x04,0x04,0x0E},{0x03,0x01,0x01,0x01,0x01,0x11,0x0E},{0x11,0x12,0x14,0x18,0x14,0x12,0x11},{0x10,0x10,0x10,0x10,0x10,0x10,0x1F},{0x11,0x1B,0x15,0x11,0x11,0x11,0x11},{0x11,0x19,0x15,0x13,0x11,0x11,0x11},{0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E},{0x1E,0x11,0x11,0x1E,0x10,0x10,0x10},{0x0E,0x11,0x10,0x10,0x10,0x11,0x0E},{0x1F,0x04,0x04,0x04,0x04,0x04,0x04},{0x11,0x11,0x11,0x11,0x11,0x11,0x0E},{0x11,0x11,0x11,0x0A,0x0A,0x04,0x04},{0x11,0x11,0x15,0x15,0x15,0x1B,0x11},{0x11,0x11,0x0A,0x04,0x0A,0x11,0x11},{0x11,0x11,0x0A,0x04,0x04,0x04,0x04},{0x1F,0x01,0x02,0x04,0x08,0x10,0x1F},
        // reste en vide
        };
        int i = (c >= 32 && c < 127) ? (c - 32) : 0;
        return G[i];
    }
    void text(int x, int y, const std::string& s, uint8_t r, uint8_t g, uint8_t b) {
        int cx = x;
        for (char c : s) {
            const uint8_t* gl = glyph(c);
            for (int gy = 0; gy < 7; gy++) for (int gx = 0; gx < 5; gx++)
                if (gl[gy] & (1 << (4 - gx))) set(cx + gx, y + gy, r, g, b);
            cx += 6;
        }
    }
    bool save_bmp(const std::string& path) {
        FILE* f = fopen(path.c_str(), "wb");
        if (!f) return false;
        int rowSize = (W * 3 + 3) & ~3;
        int imgSize = rowSize * H;
        uint8_t hdr[54] = {0};
        hdr[0] = 'B'; hdr[1] = 'M';
        uint32_t fsize = 54 + imgSize;
        uint32_t off = 54, dib = 40;
        memcpy(hdr + 2, &fsize, 4); memcpy(hdr + 10, &off, 4);
        memcpy(hdr + 14, &dib, 4);
        memcpy(hdr + 18, &W, 4); memcpy(hdr + 22, &H, 4);
        uint16_t planes = 1, bpp = 24;
        memcpy(hdr + 26, &planes, 2); memcpy(hdr + 28, &bpp, 2);
        memcpy(hdr + 34, &imgSize, 4);
        fwrite(hdr, 1, 54, f);
        std::vector<uint8_t> row(rowSize, 0);
        for (int y = H - 1; y >= 0; y--) {  // BMP = bas vers haut, BGR
            for (int x = 0; x < W; x++) {
                size_t i = ((size_t)y * W + x) * 3;
                row[x*3] = px[i+2]; row[x*3+1] = px[i+1]; row[x*3+2] = px[i];
            }
            fwrite(row.data(), 1, rowSize, f);
        }
        fclose(f);
        return true;
    }
    // ---- PNG 8 bits RGB (zlib stored blocks, pas de compression) ----
    static uint32_t crc32_update(uint32_t crc, const uint8_t* d, size_t n) {
        static uint32_t tab[256]; static bool init = false;
        if (!init) {
            for (uint32_t i = 0; i < 256; i++) {
                uint32_t c = i;
                for (int k = 0; k < 8; k++) c = (c & 1) ? 0xEDB88320u ^ (c >> 1) : c >> 1;
                tab[i] = c;
            }
            init = true;
        }
        crc ^= 0xFFFFFFFFu;
        for (size_t i = 0; i < n; i++) crc = tab[(crc ^ d[i]) & 0xFF] ^ (crc >> 8);
        return crc ^ 0xFFFFFFFFu;
    }
    static void put32(std::vector<uint8_t>& v, uint32_t x) {
        v.push_back((uint8_t)(x >> 24)); v.push_back((uint8_t)(x >> 16));
        v.push_back((uint8_t)(x >> 8)); v.push_back((uint8_t)x);
    }
    static void png_chunk(std::vector<uint8_t>& out, const char typ[4], const std::vector<uint8_t>& payload) {
        put32(out, (uint32_t)payload.size());
        size_t tpos = out.size();
        for (int i = 0; i < 4; i++) out.push_back((uint8_t)typ[i]);
        out.insert(out.end(), payload.begin(), payload.end());
        put32(out, crc32_update(0, out.data() + tpos, 4 + payload.size()));
    }
    bool save_png(const std::string& path) {
        FILE* f = fopen(path.c_str(), "wb");
        if (!f) return false;
        std::vector<uint8_t> png = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};
        std::vector<uint8_t> ihdr;
        put32(ihdr, (uint32_t)W); put32(ihdr, (uint32_t)H);
        ihdr.push_back(8); ihdr.push_back(2); ihdr.push_back(0); ihdr.push_back(0); ihdr.push_back(0); // 8 bits, RGB
        png_chunk(png, "IHDR", ihdr);
        // données brutes : filtre 0 par ligne
        std::vector<uint8_t> raw((size_t)H * (1 + (size_t)W * 3));
        for (int y = 0; y < H; y++) {
            raw[(size_t)y * (1 + (size_t)W * 3)] = 0;
            memcpy(raw.data() + (size_t)y * (1 + (size_t)W * 3) + 1, px.data() + (size_t)y * W * 3, (size_t)W * 3);
        }
        // zlib : header 0x78 0x01 + deflate stored + adler32
        std::vector<uint8_t> z;
        z.push_back(0x78); z.push_back(0x01);
        size_t p = 0;
        while (p < raw.size() || (raw.empty() && p == 0)) {
            size_t n = raw.size() - p; if (n > 65535) n = 65535;
            bool last = (p + n >= raw.size());
            z.push_back(last ? 1 : 0);                 // BFINAL, BTYPE=00
            z.push_back((uint8_t)(n & 0xFF)); z.push_back((uint8_t)(n >> 8));
            z.push_back((uint8_t)(~n & 0xFF)); z.push_back((uint8_t)((~n >> 8) & 0xFF));
            z.insert(z.end(), raw.begin() + p, raw.begin() + p + n);
            p += n;
            if (raw.empty()) break;
        }
        uint32_t ad = 1, a = 1, b = 0;
        for (uint8_t c : raw) { a = (a + c) % 65521; b = (b + a) % 65521; }
        ad = (b << 16) | a;
        put32(z, ad);
        png_chunk(png, "IDAT", z);
        png_chunk(png, "IEND", {});
        fwrite(png.data(), 1, png.size(), f);
        fclose(f);
        return true;
    }
};

#endif // BMPIMG_H_

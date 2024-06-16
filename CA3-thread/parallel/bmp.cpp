#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

struct Pixel
{
    unsigned char r, g, b;
};
#pragma pack(push, 1)
typedef struct tagBITMAPFILEHEADER
{
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
#pragma pack(pop)
using namespace std;
using namespace chrono;
int rows;
int cols;
int row;
int col;
Pixel white;

vector<vector<Pixel>> pixels;
vector<vector<Pixel>> fliped;
vector<vector<Pixel>> purpled;
vector<vector<Pixel>> filterd;
vector<Pixel> pic;

int zero = 0, one = 1, two = 2, three = 3;

void *first_flip(void *a)
{
    int part = row / 4;
    for (int i = 0; i < part; i++)
    {
        for (int j = 0; j < col; j++)
        {
            fliped[row - 1 - i][j] = pixels[i][j];
        }
    }
    pthread_exit(0);
}
void *seconde_flip(void *)
{
    int part = row / 4;
    for (int i = part; i < part * 2; i++)
    {
        for (int j = 0; j < col; j++)
        {
            fliped[row - 1 - i][j] = pixels[i][j];
        }
    }
    pthread_exit(0);
}
void *third_flip(void *)
{
    int part = row / 4;
    for (int i = 2 * part; i < part * 3; i++)
    {
        for (int j = 0; j < col; j++)
        {
            fliped[row - 1 - i][j] = pixels[i][j];
        }
    }
    pthread_exit(0);
}
void *fourth_flip(void *)
{
    int part = row / 4;
    for (int i = part; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            fliped[row - 1 - i][j] = pixels[i][j];
        }
    }
    pthread_exit(0);
}
void *par_purp(void *a)
{
    int *b = (int *)a;
    int id = *b;
    int part = row / 4;
    int last = (id == 3) ? row : (id + 1) * part;

    for (int i = id * part; i < last; i++)
    {
        for (int j = 0; j < col; j++)
        {

            Pixel a = fliped[i][j];
            int r, g, b;
            r = (a.r / 2) + ((3 * a.g) / 10) + (a.b / 2);
            g = ((a.r * 16) / 100) + ((a.g) / 2) + ((a.r * 16) / 100);
            b = ((a.r * 6) / 10) + ((a.g * 2) / 10) + ((8 * a.b) / 10);
            purpled[i][j].r = min(r, 255);
            purpled[i][j].b = min(g, 255);
            purpled[i][j].b = min(b, 255);
        }
    }
    pthread_exit(0);
}
void *par_read(void *a)
{
    int *b = (int *)a;
    int id = *b;
    int part = row / 4;
    int last = (id == 3) ? row : (id + 1) * part;

    for (int i = id * part; i < last; i++)
    {

            for (int j = 0; j < col; j++)
            {
                int of = i * col + j;
                pixels[i][j] = pic[of];
            }
        
    }
    pthread_exit(0);
}

void *par_brul(void *a)
{
    int *b = (int *)a;
    int id = *b;
    vector<unsigned char> l1 = {1, 2, 1}, l2 = {2, 4, 2}, l3 = {1, 2, 1};
    vector<vector<unsigned char>> filter;
    filter.push_back(l1);
    filter.push_back(l2);
    filter.push_back(l3);
    int part = row / 4;
    int last = (id == 3) ? row : (id + 1) * part;

    for (int i = id * part; i < last; i++)
    {
        if (i >= row - 3)
        {
            continue;
        }
        for (int j = 0; j < col - 3; j++)
        {
            int r = 0, g = 0, b = 0;
            for (int x = 0; x < 3; x++)
            {
                for (int y = 0; y < 3; y++)
                {
                    r += purpled[i + x][j + y].r * filter[x][y];
                    g += purpled[i + x][j + y].g * filter[x][y];
                    b += purpled[i + x][j + y].b * filter[x][y];
                }
            }
            filterd[i][j].r = r / 16;
            filterd[i][j].b = b / 16;
            filterd[i][j].g = g / 16;
        }
    }
    pthread_exit(0);
}
class Pic
{
private:
public:
    void write_bmp(const string &filename);

    Pic(int _row, int _col, vector<Pixel> &_pixels)
    {
        white.r = 254;
        white.g = 254;
        white.b = 254;
        row = _row;
        col = _col;
        pixels.resize(row, vector<Pixel>(col));
        fliped.resize(row, vector<Pixel>(col));
        purpled.resize(row, vector<Pixel>(col));
        filterd.resize(row, vector<Pixel>(col));
        pthread_t tids[4];
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&tids[0], &attr, par_read, &zero);
        pthread_create(&tids[1], &attr, par_read, &one);
        pthread_create(&tids[2], &attr, par_read, &two);
        pthread_create(&tids[3], &attr, par_read, &three);
        for (int i = 0; i < 4; i++)
        {
            pthread_join(tids[i], NULL);
        }
        
    }
    void flip_vertical()
    {
        pthread_t tids[4];
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&tids[0], &attr, first_flip, NULL);
        pthread_create(&tids[1], &attr, seconde_flip, NULL);
        pthread_create(&tids[2], &attr, third_flip, NULL);
        pthread_create(&tids[3], &attr, fourth_flip, NULL);
        for (int i = 0; i < 4; i++)
        {
            pthread_join(tids[i], NULL);
        }
    }

    void purp_filter()
    {

        pthread_t tids[4];
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&tids[0], &attr, par_purp, &zero);
        pthread_create(&tids[1], &attr, par_purp, &one);
        pthread_create(&tids[2], &attr, par_purp, &two);
        pthread_create(&tids[3], &attr, par_purp, &three);
        for (int i = 0; i < 4; i++)
        {
            pthread_join(tids[i], NULL);
        }
    }
    void add_hachure()
    {

        for (int i = 0; i < row; i++)
        {
            if (col - i < 0)
            {
                break;
            }
            purpled[i][col - i-1] = white;
        }
        int mid = col / 2;
        for (int i = mid; i < row; i++)
        {
            if (col - i + mid >= 0)
                purpled[i][col - i + mid-1] = white;
            if (col - i > 0)
                purpled[i - mid][col - i-1] = white;
        }
    }
    void kernel()
    {

        pthread_t tids[4];
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&tids[0], &attr, par_brul, &zero);
        pthread_create(&tids[1], &attr, par_brul, &one);
        pthread_create(&tids[2], &attr, par_brul, &two);
        pthread_create(&tids[3], &attr, par_brul, &three);

        for (int i = 0; i < 4; i++)
        {
            pthread_join(tids[i], NULL);
        }
    }
};
bool fillAndAllocate(char *&buffer, const char *fileName, int &rows, int &cols, int &bufferSize)
{
    ifstream file(fileName);
    if (!file)
    {
        cout << "File" << fileName << " doesn't exist!" << endl;
        return false;
    }

    file.seekg(0, ios::end);
    streampos length = file.tellg();
    file.seekg(0, ios::beg);

    buffer = new char[length];
    file.read(&buffer[0], length);

    PBITMAPFILEHEADER file_header;
    PBITMAPINFOHEADER info_header;

    file_header = (PBITMAPFILEHEADER)(&buffer[0]);
    info_header = (PBITMAPINFOHEADER)(&buffer[0] + sizeof(BITMAPFILEHEADER));
    rows = info_header->biHeight;
    cols = info_header->biWidth;
    bufferSize = file_header->bfSize;
    file.close();
    return true;
}

void getPixelsFromBMP24(char *buffer, int col, int row, vector<Pixel> &pixels)
{
    int row_size = col * 3;
    int padding = (4 - row_size % 4) % 4;
    int header_size = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    for (int y = row - 1; y >= 0; y--)
    {
        for (int x = 0; x < col; x++)
        {
            int offset = (y * (row_size + padding)) + (x * 3) + header_size;
            Pixel pixel = {buffer[offset + 2], buffer[offset + 1], buffer[offset]};
            pixels.push_back(pixel);
        }
    }
}
void Pic::write_bmp(const string &filename)
{
    ofstream bmp_file(filename, ios::binary);

    int row_size = col * 3;
    int padding = (4 - row_size % 4) % 4;
    int data_size = (row_size + padding) * row;
    int file_size = data_size + 54;

    bmp_file.write("BM", 2);
    bmp_file.write(reinterpret_cast<const char *>(&file_size), 4);
    bmp_file.write("\0\0\0\0", 4);
    bmp_file.write("\x36\0\0\0", 4);
    bmp_file.write("\x28\0\0\0", 4);
    bmp_file.write(reinterpret_cast<const char *>(&col), 4);
    bmp_file.write(reinterpret_cast<const char *>(&row), 4);
    bmp_file.write("\x01\0", 2);
    bmp_file.write("\x18\0", 2);
    bmp_file.write("\0\0\0\0", 4);
    bmp_file.write(reinterpret_cast<const char *>(&data_size), 4);
    bmp_file.write("\x13\x0B\0\0", 4);
    bmp_file.write("\x13\x0B\0\0", 4);
    bmp_file.write("\0\0\0\0", 4);
    bmp_file.write("\0\0\0\0", 4);
    for (int y = row - 1; y >= 0; y--)
    {
        for (int j = 0; j < col; j++)
        {
            bmp_file.write((char *)&(purpled[y][j].b), 1);
            bmp_file.write((char *)&(purpled[y][j].g), 1);
            bmp_file.write((char *)&(purpled[y][j].r), 1);
        }
        for (int i = 0; i < padding; ++i)
        {
            bmp_file.write("\0", 1);
        }
    }
    bmp_file.close();
}

int main(int argc, char *argv[])
{
    vector<unsigned char> l1 = {1, 2, 1}, l2 = {2, 4, 2}, l3 = {1, 2, 1};
    vector<vector<unsigned char>> f;
    f.push_back(l1);
    f.push_back(l2);
    f.push_back(l3);
    auto start = high_resolution_clock::now();
    char *fileBuffer;
    int bufferSize;
    if (!fillAndAllocate(fileBuffer, argv[1], rows, cols, bufferSize))
    {
        cout << "File read error" << endl;
        return 1;
    }
    getPixelsFromBMP24(fileBuffer, cols, rows, pic);
    Pic picture(rows, cols, pic);
    auto read_and_construct = high_resolution_clock::now();
    picture.flip_vertical();
    auto flip = high_resolution_clock::now();
    picture.purp_filter();
    auto purp = high_resolution_clock::now();
    picture.add_hachure();
    auto hachure = high_resolution_clock::now();
    picture.kernel();

    auto blur = high_resolution_clock::now();
    picture.write_bmp(argv[2]);
    auto write = high_resolution_clock::now();
    auto k = duration_cast<microseconds>(read_and_construct - start);
    cerr << " Read: " << (float)k.count() / 1000 << "ms" << endl;
    cerr << " Flip: " << (float)duration_cast<microseconds>(flip - read_and_construct).count() / 1000 << "ms" << endl;
    cerr << " Blur: " << (float)duration_cast<microseconds>(blur - hachure).count() / 1000 << "ms" << endl;
    cerr << " Purple: " << (float)duration_cast<microseconds>(purp - flip).count() / 1000 << "ms" << endl;
    cerr << " Lines: " << (float)duration_cast<microseconds>(hachure - purp).count() / 1000 << "ms" << endl;
    cerr << " wrtie: " << (float)duration_cast<microseconds>(write - blur).count() / 1000 << "ms" << endl;
    cerr << " Execution: " << (float)duration_cast<microseconds>(write - start).count() / 1000 << "ms" << endl;

    return 0;
}

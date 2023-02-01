#include "treasureMap.h"
#include "queue.h"
using namespace std;

treasureMap::treasureMap(const PNG & baseim, const PNG & mazeim, pair<int,int> s)
{
    start = s;
    base = baseim;
    maze = mazeim;
}


void treasureMap::setLOB(PNG & im, pair<int,int> loc, int d)
{
    RGBAPixel* pixel = im.getPixel(loc.first, loc.second);
    unsigned char d_ = (unsigned char) (d % 64);

    unsigned char rbits = (d_ & 48) >> 4;
    unsigned char gbits = (d_ & 12) >> 2;
    unsigned char bbits = d_ & 3;

    pixel->r = (pixel->r & 252) | rbits;
    pixel->g = (pixel->g & 252) | gbits;
    pixel->b = (pixel->b & 252) | bbits;
}


PNG treasureMap::renderMap()
{
    // 0. make a copy of base image
    PNG image = PNG(base);
    // 1. initialize working vectors
    vector<vector<bool>> visited (image.width(), vector<bool> (image.height()));
    vector<vector<int>> path (image.width(), vector<int> (image.height()));
    // 2. initialize queue for traversal
    Queue<pair<int, int>> Q;
    // 3. start location
    visited[start.first][start.second] = true;
    path[start.first][start.second] = 0;
    setLOB(image, start, 0);
    Q.enqueue(start);

    // 4. 
    while (!Q.isEmpty()) {
        // 4a. set curr
        pair<int, int> curr = Q.dequeue();
        vector<pair<int, int>> cneighbours = neighbours(image, curr);

        // 4b. for each compass neighbour, p, of curr
        for (pair<int, int> p : cneighbours) {
            if (good(visited, curr, p)) {
                visited[p.first][p.second] = true;
                int d = path[curr.first][curr.second] + 1;
                path[p.first][p.second] = d;
                setLOB(image, p, d);
                Q.enqueue(p);
            }
        }
    }
    // 5. return image
    return image;
}


PNG treasureMap::renderMaze()
{
    PNG img = PNG(base);
    Queue<pair<int,int>> Q;
    RGBAPixel* mazePix = maze.getPixel(start.first, start.second);
    vector<vector<bool>> visited (img.width(), vector<bool> (img.height()));

    setGrey(img, start);
    Q.enqueue(start);
    visited[start.first][start.second] = true;

    while (!Q.isEmpty()) {
        pair<int, int> curr = Q.dequeue();
        vector<pair<int,int>> cneighbours = neighbours(maze, curr);

        for (pair<int,int> p : cneighbours) {
            RGBAPixel* currPix = maze.getPixel(p.first, p.second);
            if (*currPix == *mazePix && !visited[p.first][p.second]) {
                setGrey(img, p);
                visited[p.first][p.second] = true;
                Q.enqueue(p);
            }
        }
    }

    drawSquare(img, start);

    return img;
}


bool treasureMap::good(vector<vector<bool>> & v, pair<int,int> curr, pair<int,int> next)
{
    int x = next.first;
    int y = next.second;
    if (withinBounds(maze, x, y) && !v[x][y]) {
        RGBAPixel* currpix = maze.getPixel(curr.first, curr.second);
        RGBAPixel* nextpix = maze.getPixel(x, y);
        return *currpix == *nextpix;
    }
    return false;
}


void treasureMap::setGrey(PNG & im, pair<int,int> loc)
{
    RGBAPixel* pixel = im.getPixel(loc.first, loc.second);
    pixel->r = 2 * (pixel->r / 4);
    pixel->g = 2 * (pixel->g / 4);
    pixel->b = 2 * (pixel->b / 4);
}


vector<pair<int,int>> treasureMap::neighbours(const PNG & img, pair<int,int> curr) 
{
    pair<int, int> left(curr.first - 1, curr.second);
    pair<int, int> below(curr.first, curr.second + 1);
    pair<int, int> right(curr.first + 1, curr.second);
    pair<int, int> above(curr.first, curr.second - 1);

    vector<pair<int, int>> neighbours;

    if (withinBounds(img, left.first, left.second)) neighbours.push_back(left);
    if (withinBounds(img, below.first, below.second)) neighbours.push_back(below);
    if (withinBounds(img, right.first, right.second)) neighbours.push_back(right);
    if (withinBounds(img, above.first, above.second)) neighbours.push_back(above);

    return neighbours;
}


// returns true if (x, y) coordinates of pixel is within the img
bool treasureMap::withinBounds(const PNG & img, int x, int y)
{
    int width = (int) img.width();
    int height = (int) img.height();
    return x > -1 && x < width && y > -1 && y < height;
}


void treasureMap::drawSquare(PNG & img, pair<int,int> loc)
{
    // find upper left corner of box
    int x = loc.first - 3;
    int y = loc.second - 3;

    for (int i = y; i < y + 7; i++) {
        drawHorizontalLine(img, x, i);
    }

}

void treasureMap::drawHorizontalLine(PNG & img, int x, int y) {
    for (int i = x; i < x + 7; i++) {
        if (withinBounds(img, i, y)) {
            colourRed(img, i, y);
        }
    }
}

// assumes pixel is within img range
void treasureMap::colourRed(PNG & img, int x, int y)
{
    RGBAPixel* pix = img.getPixel(x, y);
    pix->r = 255;
    pix->g = 0;
    pix->b = 0;
}

#include "decoder.h"
#include "queue.h"
#include "stack.h"
using namespace std;


decoder::decoder(const PNG & tm, pair<int,int> s) : start(s),mapImg(tm) 
{
   // initialize working vectors and queue for traversal
   vector<vector<bool>> visited (tm.width(), vector<bool> (tm.height()));
   vector<vector<int>> distance (tm.width(), vector<int> (tm.height()));
   vector<vector<pair<int,int>>> prevPt (tm.width(), vector<pair<int,int>> (tm.height()));
   Queue<pair<int,int>> Q;

   // start location
   visited[start.first][start.second] = true;
   distance[start.first][start.second] = 0;
   Q.enqueue(start);

   pair<int,int> treasure;
   while (!Q.isEmpty()) {
      pair<int,int> curr = Q.dequeue();
      vector<pair<int, int>> cneighbours = neighbours(curr);

      for (pair<int,int> next : cneighbours) {
         if (good(visited, distance, curr, next)) {
            int x = next.first;
            int y = next.second;
            visited[x][y] = true;
            distance[x][y] = distance[curr.first][curr.second] + 1;
            prevPt[x][y] = curr;
            Q.enqueue(next);
         }
      }
      if (Q.isEmpty()) treasure = curr;
   }
   buildSolutionPath(treasure, prevPt);
}


// traces back through prevPt matrix to build the solution path
void decoder::buildSolutionPath(pair<int,int> end, vector<vector<pair<int,int>>> prevPt)
{
   Stack<pair<int,int>> S;
   S.push(end);
   pair<int,int> curr = end;

   while (curr != start) {
      curr = prevPt[curr.first][curr.second];
      S.push(curr);
   }

   while (!S.isEmpty()) {
      pair<int,int> p = S.pop();
      pathPts.push_back(p);
   }
}


// returns the location of the treasure
pair<int,int> decoder::findSpot()
{
   return pathPts.back();
}


int decoder::pathLength()
{
   return pathPts.size();
}


PNG decoder::renderSolution()
{
   PNG img = PNG(mapImg);
   for (pair<int,int> p : pathPts) {
      colourRed(img, p.first, p.second);
   }
   return img;
}


PNG decoder::renderMaze()
{
   PNG img = PNG(mapImg);
   Queue<pair<int,int>> Q;
   vector<vector<bool>> visited (img.width(), vector<bool> (img.height()));
   vector<vector<int>> distance (img.width(), vector<int> (img.height()));

   setGrey(img, start);
   Q.enqueue(start);
   visited[start.first][start.second] = true;
   distance[start.first][start.second] = 0;

   while (!Q.isEmpty()) {
      pair<int,int> curr = Q.dequeue();
      vector<pair<int,int>> cneighbours = neighbours(curr);

      for (pair<int,int> next : cneighbours) {
         if (good(visited, distance, curr, next)) {
            setGrey(img, next);
            int x = next.first;
            int y = next.second;
            visited[x][y] = true;
            distance[x][y] = distance[curr.first][curr.second] + 1;
            Q.enqueue(next);
         }
      }
   }

   drawSquare(img, start);

   return img;
}


void decoder::setGrey(PNG & im, pair<int,int> loc)
{
   RGBAPixel* pixel = im.getPixel(loc.first, loc.second);
   pixel->r = 2 * (pixel->r / 4);
   pixel->g = 2 * (pixel->g / 4);
   pixel->b = 2 * (pixel->b / 4);
}


bool decoder::good(vector<vector<bool>> & v, vector<vector<int>> & d, pair<int,int> curr, pair<int,int> next)
{
   int x = next.first;
   int y = next.second;
   int dist = d[curr.first][curr.second];
   if (withinBounds(mapImg, x, y)) {
      RGBAPixel* nextPix = mapImg.getPixel(x, y);
      return !v[x][y] && compare(*nextPix, dist);
   }
   return false;
}


bool decoder::compare(RGBAPixel p, int d)
{
   int mazeVal = (p.r % 4) * 16 + (p.g % 4) * 4 + (p.b % 4);
   return mazeVal == (d + 1) % 64;
}


vector<pair<int,int>> decoder::neighbours(pair<int,int> curr) 
{
   pair<int, int> left(curr.first - 1, curr.second);
   pair<int, int> below(curr.first, curr.second + 1);
   pair<int, int> right(curr.first + 1, curr.second);
   pair<int, int> above(curr.first, curr.second - 1);

   vector<pair<int, int>> neighbours;

   neighbours.push_back(left);
   neighbours.push_back(below);
   neighbours.push_back(right);
   neighbours.push_back(above);

   return neighbours;
}


bool decoder::withinBounds(const PNG & img, int x, int y)
{
   int width = (int) img.width();
   int height = (int) img.height();
   return x > -1 && x < width && y > -1 && y < height;
}


void decoder::drawSquare(PNG & img, pair<int,int> loc)
{
   // find upper left corner of box
   int x = loc.first - 3;
   int y = loc.second - 3;

   for (int i = y; i < y + 7; i++) {
      drawHorizontalLine(img, x, i);
   }

}

void decoder::drawHorizontalLine(PNG & img, int x, int y) {
   for (int i = x; i < x + 7; i++) {
      colourRed(img, i, y);
   }
}

void decoder::colourRed(PNG & img, int x, int y)
{
   if (withinBounds(img, x, y)) {
      RGBAPixel* pix = img.getPixel(x, y);
      pix->r = 255;
      pix->g = 0;
      pix->b = 0;
   }
}

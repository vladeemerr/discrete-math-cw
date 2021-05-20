#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstdint>

enum { // NOTE: Available bit-field flags
   VERTEX_FLAG_new = (1 << 0),
   VERTEX_FLAG_opened = (1 << 1)
};

static std::vector<std::vector<std::pair<int32_t, bool>>> _g; // NOTE: Graph matrix
static std::vector<uint32_t> _flags; // NOTE: Bit-field for vertices flags
static std::vector<std::vector<int32_t>> _cycles; // NOTE: List of cycles

static void solve(int32_t a)
{
   // NOTE: Make vertices ancestor array
   std::vector<int32_t> f(_g[a].size());
   f[a] = a;
   _flags[a] |= VERTEX_FLAG_opened;

   int32_t x = a;
   // NOTE: Start iterating DFS from vertex 'x'
   while (_flags[x] & VERTEX_FLAG_opened) {
      _flags[x] &= ~VERTEX_FLAG_new;

      // NOTE: Find untraveled rib, save it to 'y'
      bool discovered = true;
      int32_t y;
      for (y = 0; y < _g[x].size(); ++y)
         if (_g[x][y].first)
            if (!_g[x][y].second) {
               discovered = false;
               break;
            }

      if (!discovered) { // NOTE: Traverse rib
         _g[x][y].second = true;
         _g[y][x].second = true;

         // NOTE: Assign 'x' ancestor of 'y'
         if (_flags[y] & VERTEX_FLAG_new) {
            _flags[y] = VERTEX_FLAG_opened;
            f[y] = x;
            x = y;
         }
         else { // NOTE: Cycle was found
            std::vector<int32_t> c;
            c.push_back(x);

            int32_t z = x;
            do { // NOTE: "Pop" ancestors and push vertices to cycle
               z = f[z];
               c.push_back(z);
            } while (z != y);

            std::reverse(c.begin(), c.end());

            _cycles.push_back(c);
         }
      }
      else { // NOTE: Can't traverse further, backing up
         _flags[x] &= ~VERTEX_FLAG_opened;
         x = f[x];
      }
   }
}

int main(int argc, char **argv)
{
   // NOTE: Open file stream from auxiliary program
   std::ifstream in(argv[1]);

   // NOTE: Read number of vertices
   int32_t vertices;
   in >> vertices;

   // NOTE: Build default graph matrix 
   _g.assign(vertices, std::vector<std::pair<int32_t, bool>>(vertices, {0, false}));
   _flags.assign(vertices, VERTEX_FLAG_new);

   // NOTE: Read graph matrix
   for (int32_t i = 0; i < vertices; ++i) {
      for (int32_t j = 0; j < vertices; ++j) {
         in >> _g[i][j].first;
      }
   }

   // NOTE: Reading is done
   in.close();

   // NOTE: Start algorithm
   for (int32_t x = 0; x < vertices; ++x)
      if (_flags[x] & VERTEX_FLAG_new)
         solve(x);

   // NOTE: Pass formatted output to auxiliary program
   std::fstream out;
   out.open(argv[1]);
   out.clear();
   out << vertices << '\n';
   for (int32_t i = 0; i < vertices; ++i) {
      for (int32_t j = 0; j < vertices; ++j) {
         out << _g[i][j].first;
         if (j != vertices)
            out << ' ';
      }
      if (i != vertices)
         out << '\n';
   }

   out << "Text:\n";
   for (int32_t i = 0; i < _cycles.size(); ++i) {
      for (int32_t j = 0; j < _cycles[i].size(); ++j) {
         out << _cycles[i][j];
         if (j != _cycles[i].size())
            out << ' ';
      }
      if (i != _cycles.size())
         out << '\n';
   }

   // NOTE: Writing is done
   out.close();
   
   return 0;
}

#include "CustomAstar.h"

/**
 * @brief Find path, through an A* impl. using tie breaking
 *
 * @param nStartX @param nStartY @param nTargetX @param nTargetY Position components
 * @param pMap The char* given in row-major order. 1 == passable cell, 0 = impassable cell
 * @param nMapWidth @param nMapHeight The map dimensions
 * @param pOutBuffer A return-param path buffer, used if return path distance < nOutBufferSize
 * @param nOutBufferSize  Max number of entries that can be written to pOutBuffer
 * @return -1 == if path not found or invalid call;
 *          0 == if start and target are the same cell;
 *          (>= 1) == if path is valid, return length of path
 **/
int FindPath(const int nStartX, const int nStartY, const int nTargetX,
             const int nTargetY, const unsigned char *pMap, const int nMapWidth,
             const int nMapHeight, int *pOutBuffer, const int nOutBufferSize) {

  // Edgecases, more initial edgecases is not be expected after this point:
  if (pOutBuffer == nullptr || pMap == nullptr) {
    return ERR_INVALID_PARAM;
  }
  if (nStartX == nTargetX && nStartY == nTargetY) {
    return NO_DISTANCE;
  }

  // Simple hashing he positions into:
  auto loc_hash = [nMapWidth](int x, int y) { return (y * nMapWidth) + x; };

  // Initial Inits before loops, but after initial edgecases:
  context curr_context{0, nMapWidth, nMapHeight, nTargetX, nTargetY};
  int32_t discovered = 0;
  bool pathing_condition = true;
  const int grid_size = nMapWidth * nMapHeight;
  const int start_pos = loc_hash(nStartX, nStartY),
            target_pos = loc_hash(nTargetX, nTargetY);

  // Initial Declarations, path vector and a minheap priority queue:
  std::vector<int> path(grid_size), distance(grid_size, MAX_INT);
  std::priority_queue<truple, std::vector<truple>, std::greater<truple>> ordered_paths;

  // Initial assignments:
  distance[start_pos] = 0;
  curr_context.pos = start_pos;
  ordered_paths.push(std::make_tuple(0 + CalcHBound(&curr_context), 0, start_pos));

  // Actual A*, loop-body:
  while (likely(!ordered_paths.empty() && pathing_condition)) {
    int current_pos = std::get<2>(ordered_paths.top());
    ordered_paths.pop();
    VISITED_CELLS++;

    // 4 steps (directions) of the following for-loop per each while step:
    for (auto edge : {+1, -1, +nMapWidth, -nMapWidth}) {
      const int next_pos = current_pos + edge;
      // Edge detection:
      if (unlikely((edge == 1 && (next_pos % nMapWidth == 0)) ||
                   (edge == -1 && (current_pos % nMapWidth == 0)))) {
        continue; // Skip if current step at edge
      }

      // Pathing / Path-detection:
      if (likely(0 <= next_pos && next_pos < grid_size && distance[next_pos] > (distance[current_pos] + 1) && pMap[next_pos])) {
        distance[next_pos] = distance[current_pos] + 1;
        path[next_pos] = current_pos;

        if (unlikely(next_pos == target_pos)) {
          pathing_condition = false;
          break; // found target position!
        }

        // Didn't find target, store step in path:
        curr_context.pos = next_pos;
        ordered_paths.push( std::make_tuple(distance[next_pos] + CalcHBound(&curr_context), ++discovered, next_pos));
      }
    }
  }
  // End of main A* loop
  // Return either valid or invalid path and distance
  if (distance[target_pos] <= nOutBufferSize) {
    // Storing the shortest path in pOutBuffer
    int curr = target_pos;
    for (int step = distance[target_pos] - 1; step >= 0; step--) {
      pOutBuffer[step] = curr;
      curr = path[curr];
    }
    return distance[target_pos];
  } else if (distance[target_pos] == MAX_INT) {return ERR_PATH_NOT_FOUND;}

  // Path buffer not large enough, only return shortest distance
  return distance[target_pos];
}

/**
 * @brief Fake absolute value
 *
 * NOTE: Not worth adding the full math lib for only one function,
 * so I made a simple, branchless, fake abs
 *
 * @param iinput Integer to get absolute value from
 * @return absolute value
 **/
int FauxAbs(int iinput) {
  return (iinput * (iinput >= 0)) + (iinput * -(iinput < 0));
}

/**
 * @brief Calculates h distance, Manhattan distance from position to target.
 *
 * @param pCon context ptr to fetch values through
 * @return H distance, Lower-bound
 **/
int CalcHBound(context *pCon) {
  int x = pCon->pos % pCon->nMapWidth, y = pCon->pos / pCon->nMapWidth;
  return FauxAbs(x - pCon->nTargetX) + FauxAbs(y - pCon->nTargetY);
}

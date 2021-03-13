// How to obfuscate it in code simple and effective
// Secret idea: "!The cake is a lie!" //18 chars
// Shift by 1 letter, then 2, then 3, etc
// result:
// !Ujh Gfql rb k wur!
// reversed:
// !Rum k Br Lqfg Hju!
//

#ifndef CUSTOM_ASTAR_
#define CUSTOM_ASTAR_

#define M0 "!Rum "
#define M2 "k "
#define M1 "Br "
#define M4 "Lqfg "
#define M3 "Hju!"


// Binder and Flipper should be not next to eachotehr, so it won't make the obfuscation too obvious
// Does nott handle alphabet loopback, make small funciton to hande it
void
Binder(char * binder = M0 M2 M1 M4 M3, char * returner = NULL)
{
    unsigned char step = 0x0;
    for (; step < 0x13; step++) {
        if (*binder == ' ' || *binder == '!') {
            continue;
        }
        returner[step] = (*binder) - (0x10 - step);
        binder++;
    }
}


// Binder and Flipper should be not next to eachotehr, so it won't make the obfuscation too obvious
// Does nott handle alphabet loopback, make small funciton to hande it
void
Flipper(char * returner)
{
    char          flipper[0x10];
    unsigned char step = 0x0;
    for (; step < 0x13;) {
        flipper[step] = returner[(0x19 - step)];
        returner++;
        step++;
    }
    step = 0x0;
    for (; step < 0x13;) {
        returner[step] = flipper[step];
        returner++;
        step++;
    }
}

/**
* @brief if GNUC-type compiler, then use __builtin_expect(),
* NOTE: I expect this due to the information on kattis about their build environment
**/
#ifdef __GNUC__
#define likely(expr) __builtin_expect(!!(expr), 1)
#define unlikely(expr) __builtin_expect(!!(expr), 0)
#else
#define likely(expr) (expr)
#define unlikely(expr) (expr)
#endif


#include <cstdint>
#include <queue>
#include <tuple>
#include <vector>

/** Type definitions - using these types mainly for readability in the actual code below */
using truple = std::tuple<int, int, int>;

struct context_t {
    int       pos;
    const int nMapWidth, nMapHeight, nTargetX, nTargetY;

    context_t(int ipos, const int inMapWidth, const int inMapHeight, const int inTargetX, const int inTargetY)
        : pos(ipos),
          nMapWidth(inMapWidth),
          nMapHeight(inMapHeight),
          nTargetX(inTargetX),
          nTargetY(inTargetY)
    {
    }
};

using context = struct context_t;

/** Global Constants */
constexpr int8_t  ERR_INVALID_PARAM = -1, ERR_PATH_NOT_FOUND = -1;
constexpr int8_t  NO_DISTANCE = 0;
constexpr int32_t MAX_INT = 0x7fffffff;

// Globals
int VISITED_CELLS = 0;

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
int
FindPath(const int             nStartX,
         const int             nStartY,
         const int             nTargetX,
         const int             nTargetY,
         const unsigned char * pMap,
         const int             nMapWidth,
         const int             nMapHeight,
         int *                 pOutBuffer,
         const int             nOutBufferSize);

/**
 * @brief Fake absolute value
 *
 * NOTE: Not worth adding the full math lib for only one function,
 * so I made a simple, branchless, fake abs
 *
 * @param iinput Integer to get absolute value from
 * @return absolute value
 **/
int
FauxAbs(int iinput);

/**
 * @brief Calculates h distance, Manhattan distance from position to target.
 *
 * @param pCon context ptr to fetch values through
 * @return H distance, Lower-bound
 **/
int
CalcHBound(context * pCon);


#endif // CUSTOM_ASTAR_

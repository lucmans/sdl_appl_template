#include "graphics/graphics_funcs.hpp"

#include <SDL2/SDL.h>

#include <cmath>  // round()


/* given a dst where we need to fit src in, there are 3 cases:
 *   1: src is wider/shorter than dst (src_ar > dst_ar)
 *      ┌─────────┐
 *      ├─────────┤ in this case:
 *      │         │    we set the src.x and src.w to dst minus padding
 *      ├─────────┤    calculate corresponding src.h using src_ar and center
 *      └─────────┘
 *
 *   2: src is narrower/longer than dst (src_ar < dst_ar)
 *      ┌──┬───┬──┐
 *      │  │   │  │ in this case:
 *      │  │   │  │    we set the src.x and src.w to dst minus padding
 *      │  │   │  │    calculate corresponding src.w using src_ar and center
 *      └──┴───┴──┘
 *
 *   3: the aspect ratio of src and dst are equal
 *      set src to dst minus padding
 */
void fit_center(SDL_Rect& src, const SDL_Rect& dst, const int padding /*= 0*/) {
    const double src_ar = (double)src.w / src.h,
                 dst_ar = (double)dst.w / dst.h;

    // case 1: dst is too narrow/long for src
    if (src_ar > dst_ar) {
        src.w = dst.w - (padding * 2);
        src.h = (int)std::round(src.w / src_ar);
        src.x = dst.x + padding;
        src.y = dst.y + padding + (int)std::round((dst.h - src.h) / 2.0);
    }
    // case 2: dst is too wide/short for src
    else if (src_ar < dst_ar) {
        src.h = dst.h - (padding * 2);
        src.w = (int)std::round(src.h * src_ar);
        src.x = dst.x + padding + (int)std::round((dst.w - src.w) / 2.0);
        src.y = dst.y + padding;
    }
    else {  // case 3: src_ar == dst_ar
        src.w = dst.w - (2 * padding);
        src.h = src.h - (2 * padding);
        src.x = padding;
        src.y = padding;
    }

    // TODO: check if result is valid and throw exception if not
}

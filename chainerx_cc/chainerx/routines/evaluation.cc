#include "chainerx/routines/evaluation.h"

#include <cmath>
#include <cstddef>
#include <cstdint>

#include <absl/types/optional.h>

#include "chainerx/array.h"
#include "chainerx/routines/arithmetic.h"
#include "chainerx/routines/creation.h"
#include "chainerx/routines/indexing.h"
#include "chainerx/routines/logic.h"
#include "chainerx/routines/manipulation.h"
#include "chainerx/routines/reduction.h"
#include "chainerx/routines/sorting.h"
#include "chainerx/routines/statistics.h"
#include "chainerx/routines/type_util.h"
#include "chainerx/scalar.h"
#include "chainerx/shape.h"

namespace chainerx {

Array Accuracy(const Array& y, const Array& t, const absl::optional<int8_t>& ignore_label) {
    if (ignore_label.has_value()) {
        Array ignore = chainerx::FullLike(t, Scalar{*ignore_label});
        Array mask = Equal(t, ignore);
        Array ignore_cnt = Sum(mask);
        Array pred = Where(mask, ignore, ArgMax(y, 1).Reshape(t.shape()));
        Array count = Sum(Equal(pred, t)) - ignore_cnt;
        Scalar size{t.GetTotalSize()};
        Scalar total = size - AsScalar(ignore_cnt);
        if (total == 0.0) {
            return Array{0}.AsType(y.dtype());
        } else {
            return Divide(count, total).AsType(y.dtype());
        }
    } else {
        Array pred = ArgMax(y, 1).Reshape(t.shape());
        return Mean(Equal(pred, t).AsType(y.dtype()));
    }
}

}  // namespace chainerx

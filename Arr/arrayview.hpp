//
//  array.hpp
//  Arr
//
//  Created by DB on 11/21/18.
//  Copyright © 2018 D Blalock. All rights reserved.
//

#ifndef arrayview_h
#define arrayview_h

#include <array>
#include <assert.h>

#include "bint.hpp"
#include "macros.hpp" // for SFINAE macros

#include "array_utils.hpp" // for debug

// /** elements of tensor are contiguous along this ax with stride of 1;
//  * e.g., axis 1 in a row-major matrix
//  */
// class ContiguousAx {
//     static const bool is_contig = true;
//     static const bool is_dense = true;
//     static const bool is_strided = true;
// };
/** elements of tensor are not contiguous along this ax, but
 * sub-arrays are; e.g., axis 0 in a row-major matrix
 *
 * Dense along ax0:
 * x x x x
 * x x x x
 * x x x x
 *
 * Not dense along ax0, but still strided:
 *
 * x x x x
 *
 * x x x x
 *
 * x x x x
 *
 * Not strided (ie, with uniform stride) along ax0:
 *
 * x x x x
 * x x x x
 *
 * x x x x
 *
 * Dense along ax0, but only strided along ax1:
 *
 * x   x   x   x
 * x   x   x   x
 * x   x   x   x
 */
// template<int Used=true>
// class DenseAx {
//     static const int is_used = Used;
//     static const bool is_contig = false;
//     static const bool is_dense = true;
//     static const bool is_strided = true;
// };
// template<int Used=false> class StridedAx {
//     static const int is_used = Used;
//     static const bool is_contig = false;
//     static const bool is_dense = false;
//     static const bool is_strided = true;
// };
// // for stuff like where(); not sure how to support this
// template<int Used=false> class ChaosAx {
//     static const int is_used = Used;
//     static const bool is_contig = false;
//     static const bool is_dense = false;
//     static const bool is_strided = false;
// };

using DefaultIndexType = int32_t;

// ================================================================ Axis

template<bool Contiguous=false, bool Dense=true, bool Strided=true,
    int Used=true, int StaticStride=0, class SizeBounds=NoBounds>
struct Axis {
    static const bool is_contig = Contiguous;
    static const bool is_dense = Dense;
    static const bool is_strided = Strided;
    static const int is_used = Used;
    static const int stride = StaticStride;
    using size_bounds = SizeBounds;
};

using AxisContig = Axis<true, true, true, true, 1>;
using AxisDense = Axis<false, true, true, true>;
using AxisStrided = Axis<false, false, true, true>;
using AxisUnused = Axis<false, true, true, false>;

// using type = Axis<AxisT::is_contig, AxisT::is_dense, AxisT::is_strided,
//         AxisT::is_used, AxisT::stride, typename AxisT::SizeBounds>;

template<class AxisT, bool Contig> struct setAxisContiguous {
    using type = Axis<Contig, AxisT::is_dense, AxisT::is_strided,
        AxisT::is_used, AxisT::stride, typename AxisT::SizeBounds>;
};
template<class AxisT, bool Dense> struct setAxisDense {
    using type = Axis<AxisT::is_contig, Dense, AxisT::is_strided,
        AxisT::is_used, AxisT::stride, typename AxisT::SizeBounds>;
};
template<class AxisT, bool Strided> struct setAxisStrided {
    using type = Axis<AxisT::is_contig, AxisT::is_dense, Strided,
        AxisT::is_used, AxisT::stride, typename AxisT::SizeBounds>;
};
template<class AxisT, int Used> struct setAxisUsed {
    using type = Axis<AxisT::is_contig, AxisT::is_dense, AxisT::is_strided,
        Used, AxisT::stride, typename AxisT::SizeBounds>;
};
template<class AxisT, int Stride> struct setAxisStaticStride {
    using type = Axis<AxisT::is_contig && (Stride == 1), AxisT::is_dense,
    AxisT::is_strided, AxisT::is_used, Stride, typename AxisT::SizeBounds>;
};
template<class AxisT, class SizeBounds> struct setAxisSizeBounds {
    using type = Axis<AxisT::is_contig, AxisT::is_dense, AxisT::is_strided,
        AxisT::is_used, AxisT::stride, SizeBounds>;
};

template<int size> struct SizeBoundsForStaticSize {
    static const int valid = size > 0;
    using type = bint<valid ? size : NoBounds::max,
                      valid ? size : NoBounds::min, valid>;
};

template<class AxisT, int StaticSize> struct setAxisStaticSize {
    using SizeBoundsT = typename SizeBoundsForStaticSize<StaticSize>::type;
    using type = typename setAxisSizeBounds<AxisT, SizeBoundsT>::type;
    // using type = Axis<AxisT::is_contig, AxisT::is_dense, AxisT::is_strided,
    //     AxisT::is_used, AxisT::stride,
    //     typename SizeBoundsForStaticSize<StaticSize>::type>;
};


#define SET_AXIS_PROP(STRUCT_NAME, AXES_T, AX, VAL) \
    typename STRUCT_NAME<GET_AXIS_T(AXES_T, AX), VAL>::type

#define SET_AXIS_CONTIGUOUS(AXES_T, AX, BOOL) \
    SET_AXIS_PROP(setAxisContiguous, AXES_T, AX, BOOL)

#define SET_AXIS_DENSE(AXES_T, AX, BOOL) \
    SET_AXIS_PROP(setAxisDense, AXES_T, AX, BOOL)

#define SET_AXIS_STRIDED(AXES_T, AX, BOOL) \
    SET_AXIS_PROP(setAxisStrided, AXES_T, AX, BOOL)

#define SET_AXIS_USED(AXES_T, AX, BOOL) \
    SET_AXIS_PROP(setAxisUsed, AXES_T, AX, BOOL)
    // typename setAxisContiguous(GET_AXIS_T(AXES_T, AX), BOOL)::type

#define SET_AXIS_STRIDE(AXES_T, AX, BOOL) \
    SET_AXIS_PROP(setAxisStaticStride, AXES_T, AX, BOOL)

#define SET_AXIS_SIZE(AXES_T, AX, BOOL) \
    SET_AXIS_PROP(setAxisStaticSize, AXES_T, AX, BOOL)


// template<int size> struct SizeBoundsForSize { using type = NoBounds; };

// ================================================================ Axes

// ------------------------------------------------ axis type

template<class Ax0=AxisContig, class Ax1=AxisUnused,
    class Ax2=AxisUnused, class Ax3=AxisUnused >
struct Axes {
    static const int rank =
        Ax0::is_used + Ax1::is_used + Ax2::is_used + Ax3::is_used;
    static const bool is_any_ax_contig =
        Ax0::is_contig || Ax1::is_contig || Ax2::is_contig || Ax3::is_contig;
    static const bool is_dense =
        Ax0::is_dense && Ax1::is_dense && Ax2::is_dense && Ax3::is_dense;
    using AxisT0 = Ax0;
    using AxisT1 = Ax1;
    using AxisT2 = Ax2;
    using AxisT3 = Ax3;

    // is_dense -> one ax must be contiguous, or this makes no sense
    static_assert(!is_dense || is_any_ax_contig,
        "Somehow dense, but nothing contiguous!?");
};

template<int ax, class Axes> struct getAxis {};
template<class Axes> struct getAxis<0, Axes> { using type = typename Axes::AxisT0; };
template<class Axes> struct getAxis<1, Axes> { using type = typename Axes::AxisT1; };
template<class Axes> struct getAxis<2, Axes> { using type = typename Axes::AxisT2; };
template<class Axes> struct getAxis<3, Axes> { using type = typename Axes::AxisT3; };

#define GET_AXIS_T(AXES, INT) typename getAxis<INT, AXES>::type

// template<int Ax, class Axis> struct getAxis {};
// template<class Axis>

// ------------------------------------------------ aliases for common axes

using AxesDense1D =
    Axes<AxisContig, AxisUnused, AxisUnused, AxisUnused>;

using AxesRowMajor2D =
    Axes<AxisDense, AxisContig, AxisUnused, AxisUnused>;
using AxesRowMajor3D =
    Axes<AxisStrided, AxisDense, AxisContig, AxisUnused>;
using AxesRowMajor4D =
    Axes<AxisDense, AxisDense, AxisDense, AxisContig>;

using AxesColMajor2D =
    Axes<AxisContig, AxisDense, AxisUnused, AxisUnused>;
using AxesColMajor3D =
    Axes<AxisContig, AxisDense, AxisDense, AxisUnused>;
using AxesColMajor4D =
    Axes<AxisContig, AxisDense, AxisDense, AxisDense>;

using AxesNCHW =
    Axes<AxisDense, AxisDense, AxisContig, AxisDense>;

// ------------------------------------------------ StorageOrder

struct StorageOrders { enum { RowMajor, ColMajor, NCHW, Unspecified }; };

template<int Order> struct StorageOrder { };
template<> struct StorageOrder<StorageOrders::RowMajor> {
    static constexpr std::array<int, 4> order {3, 2, 1, 0};
};

// ------------------------------------------------ strides for various axes

template<class AxesT, class IdxT=DefaultIndexType,
    int Order=StorageOrders::Unspecified>
std::array<IdxT, AxesT::rank> default_strides_for_shape(
    std::array<IdxT, AxesT::rank> shape)
{
    static const int rank = AxesT::rank;
    static_assert(rank >= 0, "Rank must be >= 0!");
    static_assert(rank <= 4, "Rank must be <= 4!");
    static_assert(AxesT::is_dense, "Only dense axes can use default strides!");
    static_assert(rank <= 2 || Order != StorageOrders::Unspecified,
        "Must specify storage order for rank 3 tensors and above!");
    static_assert((Order == StorageOrders::RowMajor ||
            Order == StorageOrders::ColMajor ||
            Order == StorageOrders::NCHW ||
            Order == StorageOrders::Unspecified),
        "Only StorageOrders RowMajor, ColMajor, NCHW, "
            "and Unspecified supported!");
    static_assert(rank != 4 || Order==StorageOrders::NCHW,
        "NCHW order only supported for rank 4 tensors!");

    std::array<IdxT, AxesT::rank> strides{0};
    if (rank == 1) {
        // static_assert(AxisT0::is_contig,
        //     "1D array must be contiguous to use default strides!");
        strides[0] = 1;
        return strides;
    }
    if (rank == 2) {
        if (AxesT::AxisT0::is_contig) { // colmajor
            strides[0] = 1;
            strides[1] = shape[0];
        } else { // rowmajor
            strides[0] = shape[1];
            strides[1] = 1;
        }
        return strides;
    }
    switch(Order) { // rank 3 or 4 if we got to here
        case StorageOrders::RowMajor:
            strides[rank - 1] = 1;
            for (int i = rank - 2; i >= 0; i--) {
                strides[i] = shape[i + 1] * strides[i + 1];
            }
            break;
        case StorageOrders::ColMajor:
            strides[0] = 1;
            for (int i = 1; i < rank; i++) {
                strides[i] = shape[i - 1] * strides[i - 1];
            }
            break;
        case StorageOrders::NCHW:
            // conceptually, axes mean NHWC: #imgs, nrows, ncols, nchannels
            strides[0] = shape[1] * shape[2] * shape[3]; // sz of whole img
            strides[1] = shape[2];  // row stride = number of cols
            strides[2] = 1;         // col stride = 1, like rowmajor
            strides[3] = shape[1] * shape[2];  // channel stride = nrows * ncols
            break;
        default:
            assert("Somehow got unrecognized storage order!");
            break; // can't happen
    }
    return strides;
}

// ================================================================ IdxSet

template<class IdxT, REQUIRE_INT(IdxT)>
struct IdxSet4 {
    IdxSet4(IdxT idx0, IdxT idx1, IdxT idx2, IdxT idx3):
        idxs{idx0, idx1, idx2, idx3} {};
    std::array<IdxT, 4> idxs;
        // i0(idx0), i1(idx1), i2(idx2), i3(idx3) {};
    // IdxT i0, i1, i2, i3;
};
template<class IdxT, REQUIRE_INT(IdxT)>
struct IdxSet3 {
    IdxSet3(IdxT idx0, IdxT idx1, IdxT idx2):
        idxs{idx0, idx1, idx2} {};
    std::array<IdxT, 3> idxs;
    //     i0(idx0), i1(idx1), i2(idx2) {};
    // IdxT i0, i1, i2;
};
template<class IdxT, REQUIRE_INT(IdxT)>
struct IdxSet2 {
    IdxSet2(IdxT idx0, IdxT idx1):
        idxs{idx0, idx1} {};
    std::array<IdxT, 2> idxs;
    // IdxSet2(IdxT idx0, IdxT idx1=0):
    //     i0(idx0), i1(idx1) {};
    // IdxT i0, i1;
};
template<class IdxT, REQUIRE_INT(IdxT)>
struct IdxSet1 {
    IdxSet1(IdxT idx0):
        idxs({idx0}) {};
    std::array<IdxT, 1> idxs;
    // IdxSet1(IdxT idx0):
    //     i0(idx0) {};
    // IdxT i0;
};

// enum class StorageOrder : std::array<int, 4> {
//     RowMajor = std::array<int, 4>{3, 2, 1, 0};
//     ColMajor = std::array<int, 4>{0, 1, 2, 3};
//     NCHW = std::array<int, 4>{3, 1, 0, 2};
//     Unspecified = std::array<int, 4>{-1, -1, -1, -1};
// };

template<int rank, class IdxT> struct IdxSet {};
template<class IdxT> struct IdxSet<1, IdxT> { using type = IdxSet1<IdxT>; };
template<class IdxT> struct IdxSet<2, IdxT> { using type = IdxSet2<IdxT>; };
template<class IdxT> struct IdxSet<3, IdxT> { using type = IdxSet3<IdxT>; };
template<class IdxT> struct IdxSet<4, IdxT> { using type = IdxSet4<IdxT>; };

template<class DataT, class AxesT=AxesDense1D, class IdxT=DefaultIndexType>
struct ArrayView {
    static const int rank = AxesT::rank;
    using axis_t = AxesT;
    using strides_t = std::array<IdxT, rank>;
    using shape_t = std::array<IdxT, rank>;
    using idxset_t = typename IdxSet<rank, IdxT>::type;

    // static const bool is_contig = Ax1::is_contig

    ArrayView(DataT *const data, const std::array<IdxT, rank> shape):
        _data(data),
        _shape(shape),
        _strides({1})
    {
        printf("I have rank %d\n", rank);
    };

    // DataT& operator[](IdxT i0, IdxT i1, IdxT i2, IdxT i3) {
    DataT& operator[](const idxset_t idxs) {
        // printf("idxs: %d, %d, %d, %d\n", idxs.i0, idxs.i1, idxs.i2, idxs.i3);
        ar::print(idxs.idxs.data(), rank, "idxs");
        ar::print(_shape.data(), rank, "shape");
        ar::print(_strides.data(), rank, "strides");
        // printf("shape: %d, %d, %d, %d\n", idxs.i0, idxs.i1, idxs.i2, idxs.i3);
        // printf("strides: %d, %d, %d, %d\n", idxs.i0, idxs.i1, idxs.i2, idxs.i3);
        return _data[0];
        // TODO uncomment
        // return _data[idxs.i0 * _strides[0] + idxs.i1 * _strides[1]
        //            + idxs.i2 * _strides[1] + idxs.i3 * _strides[3]];
    }
    // const DataT& operator[](const IdxSet<IdxT, 4> idxs) {
    //     return _data + idxs.i0 * _strides[0] + idxs.i1 * _strides[1]
    //                  + idxs.i2 * _strides[1] + idxs.i3 * _strides[3];
    // }

private:
    DataT *const _data;
    const shape_t _shape;
    const strides_t _strides;
};

template<class AxesT, int StaticDim0=0, int StaticDim1=0, int StaticDim2=0, int StaticDim3=0>
struct setStaticSizes {
    using AxisT0 = SET_AXIS_SIZE(AxesT, 0, StaticDim0);
    using AxisT1 = SET_AXIS_SIZE(AxesT, 1, StaticDim1);
    using AxisT2 = SET_AXIS_SIZE(AxesT, 2, StaticDim2);
    using AxisT3 = SET_AXIS_SIZE(AxesT, 3, StaticDim3);
    using type = Axes<AxisT0, AxisT1, AxisT2, AxisT3>;
};

template<int Rank, int Order> struct GetDefaultAxesType {};
template<> struct GetDefaultAxesType<1, StorageOrders::RowMajor> { using type = AxesDense1D; };
template<> struct GetDefaultAxesType<1, StorageOrders::ColMajor> { using type = AxesDense1D; };
template<> struct GetDefaultAxesType<2, StorageOrders::RowMajor> { using type = AxesRowMajor2D; };
template<> struct GetDefaultAxesType<2, StorageOrders::ColMajor> { using type = AxesColMajor2D; };
template<> struct GetDefaultAxesType<3, StorageOrders::RowMajor> { using type = AxesRowMajor3D; };
template<> struct GetDefaultAxesType<3, StorageOrders::ColMajor> { using type = AxesColMajor3D; };
template<> struct GetDefaultAxesType<4, StorageOrders::RowMajor> { using type = AxesRowMajor4D; };
template<> struct GetDefaultAxesType<4, StorageOrders::ColMajor> { using type = AxesColMajor4D; };
template<> struct GetDefaultAxesType<4, StorageOrders::NCHW>     { using type = AxesNCHW; };

template<int Rank, int Order, int StaticDim0, int StaticDim1,
    int StaticDim2, int StaticDim3>
struct GetAxesType {
    using baseAxesType = typename GetDefaultAxesType<Rank, Order>::type;
    using type = typename setStaticSizes<
        baseAxesType, StaticDim0, StaticDim1, StaticDim2, StaticDim3>::type;
};

// template<class DataT, int Rank, int Order, class IdxT> struct GetArrayViewType {
//     using type = ArrayView<DataT,
//         typename GetDefaultAxesType<Rank, Order>::type, IdxT>;
// };

// template<int Order=StorageOrders::RowMajor,
//     int StaticDim0=0, int StaticDim1=0, int StaticDim=0, int StaticDim3=0>

template<int Rank, int Order=StorageOrders::RowMajor,
    int StaticDim0=0, int StaticDim1=0, int StaticDim2=0, int StaticDim3=0,
    class IdxT=DefaultIndexType, class DataT=void>
struct GetArrayViewType {
    using AxesT = typename GetAxesType<Rank, Order,
        StaticDim0, StaticDim1, StaticDim2, StaticDim3>::type;
    using type = ArrayView<DataT, AxesT, IdxT>;
};

template<int Order=StorageOrders::RowMajor,
    int StaticDim0=0, int StaticDim1=0, int StaticDim2=0, int StaticDim3=0,
    class IdxT=DefaultIndexType, class DataT=void>
auto make_view(DataT* data, IdxT dim0, IdxT dim1, IdxT dim2, IdxT dim3)
    -> GetArrayViewType<4, Order,
        StaticDim0, StaticDim1, StaticDim2, StaticDim3, IdxT, DataT>
{
    using ArrayViewT = GetArrayViewType<4, Order,
        StaticDim0, StaticDim1, StaticDim2, StaticDim3, IdxT, DataT>;

    return ArrayViewT(data);






    // SELF: pick up here by enabling passing in shape
    //      -maybe do this by actually just passing in std::array as the shape
    // -also make it possible to set strides on array view, ideally





}



#endif /* array_h */

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

template<bool Contiguous=false, bool Dense=true, bool Strided=true, int Used=true>
struct Axis {
    static const int is_used = Used;
    static const bool is_contig = Contiguous;
    static const bool is_dense = Dense;
    static const bool is_strided = Strided;
};

using AxisContig = Axis<true, true, true, true>;
using AxisDense = Axis<false, true, true, true>;
using AxisStrided = Axis<false, false, true, true>;
using AxisUnused = Axis<false, true, true, false>;

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


template<int Rank, int Order> struct GetAxesType {};
template<> struct GetAxesType<1, StorageOrders::RowMajor> { using type = AxesDense1D; };
template<> struct GetAxesType<1, StorageOrders::ColMajor> { using type = AxesDense1D; };
template<> struct GetAxesType<2, StorageOrders::RowMajor> { using type = AxesRowMajor2D; };
template<> struct GetAxesType<2, StorageOrders::ColMajor> { using type = AxesColMajor2D; };
template<> struct GetAxesType<3, StorageOrders::RowMajor> { using type = AxesRowMajor3D; };
template<> struct GetAxesType<3, StorageOrders::ColMajor> { using type = AxesColMajor3D; };
template<> struct GetAxesType<4, StorageOrders::RowMajor> { using type = AxesRowMajor4D; };
template<> struct GetAxesType<4, StorageOrders::ColMajor> { using type = AxesColMajor4D; };
template<> struct GetAxesType<4, StorageOrders::NCHW>     { using type = AxesNCHW; };


template<class DataT, int Rank, int Order, class IdxT> struct GetArrayViewType {
    using type = ArrayView<DataT, typename GetAxesType<Rank, Order>::type, IdxT>;
};


template<int Order=StorageOrders::RowMajor,
    int StaticDim0=0, int StaticDim1=0, int StaticDim=0, int StaticDim3=0,
    class IdxT=DefaultIndexType, class DataT=void>
auto make_view(DataT* data, IdxT dim0, IdxT dim1, IdxT dim2, IdxT dim3)
    -> GetArrayViewType<DataT, 4, Order, IdxT>
{
    using AxisT = typename GetAxesType<4, Order>::type;
}



#endif /* array_h */

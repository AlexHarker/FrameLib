
#ifndef FRAMELIB_EDGES_H
#define FRAMELIB_EDGES_H

#include "../../FrameLib_Dependencies/TableReader.hpp"

// Enum for options (if implementing all)

enum Edges { kEdgePad, kEdgeExtend, kEdgeWrap, kEdgeFold, kEdgeMirror };

// Underlying fetch class

struct EdgeFetch : table_fetcher<double>
{
    EdgeFetch(const double *data, unsigned long size)
    : table_fetcher<double>(static_cast<intptr_t>(size), 1.0), mData(data) {}
    
    double operator()(intptr_t idx) { return mData[idx]; }
    
    const double *mData;
};

// Base to hide unneeded functionality

template <template <class T> class Fetch>
struct EdgesBase
{
    EdgesBase(const double *data, unsigned long size)
    : mFetch(EdgeFetch(data, size)) {}
    
    double operator()(intptr_t idx) { return mFetch(idx); }
    
private:
    
    Fetch<EdgeFetch> mFetch;
};

// Edge types (padding type is explicit / others use EdgesBase via TableReader.hpp adaptors)

struct EdgesPad : private EdgeFetch
{
    EdgesPad(const double *data, unsigned long size, double padding)
    : EdgeFetch(data, size), mPadding(padding) {}
    
    double operator()(intptr_t idx)
    {
        return (idx >= 0 && idx < EdgeFetch::size) ? mData[idx] : mPadding;
    }
    
private:
    
    double mPadding;
};

using EdgesExtend   = EdgesBase<table_fetcher_extend>;
using EdgesWrap     = EdgesBase<table_fetcher_wrap>;
using EdgesFold     = EdgesBase<table_fetcher_fold>;
using EdgesMirror   = EdgesBase<table_fetcher_mirror>;

#endif /* FRAMELIB_EDGES_H */

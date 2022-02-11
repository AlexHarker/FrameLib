
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

// Edge types (padding type is explicit / others use TableReader.hpp)

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

struct EdgesExtend : EdgesBase<table_fetcher_extend>
{
    EdgesExtend(const double *data, unsigned long size)
    : EdgesBase<table_fetcher_extend>(data, size) {}
};

struct EdgesWrap : EdgesBase<table_fetcher_wrap>
{
    EdgesWrap(const double *data, unsigned long size)
    : EdgesBase<table_fetcher_wrap>(data, size) {}
};

struct EdgesFold : EdgesBase<table_fetcher_fold>
{
    EdgesFold(const double *data, unsigned long size)
    : EdgesBase<table_fetcher_fold>(data, size) {}
};

struct EdgesMirror : EdgesBase<table_fetcher_mirror>
{
    EdgesMirror(const double *data, unsigned long size)
    : EdgesBase<table_fetcher_mirror>(data, size) {}
};

#endif /* FRAMELIB_EDGES_H */

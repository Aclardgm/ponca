#pragma once

#include <PCA/SpacePartitioning/Query/RangeIndexQuery.h>
#include <PCA/SpacePartitioning/KdTree/Query/KdTreeQuery.h>
#include <PCA/SpacePartitioning/KdTree/Iterator/KdTreeRangeIndexIterator.h>

namespace Ponca {

class KdTreeRangeIndexQuery : public KdTreeQuery,
                              public RangeIndexQuery
{
protected:
    friend class KdTreeRangeIndexIterator;

public:
    KdTreeRangeIndexQuery();
    KdTreeRangeIndexQuery(const KdTree* kdtree);
    KdTreeRangeIndexQuery(const KdTree* kdtree, Scalar radius);
    KdTreeRangeIndexQuery(const KdTree* kdtree, Scalar radius, int index);

public:
    KdTreeRangeIndexIterator begin();
    KdTreeRangeIndexIterator end();

protected:
    void initialize(KdTreeRangeIndexIterator& iterator);
    void advance(KdTreeRangeIndexIterator& iterator);
};

}   
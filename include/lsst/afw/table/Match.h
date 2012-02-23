// -*- lsst-c++ -*-

/* 
 * LSST Data Management System
 * Copyright 2008, 2009, 2010 LSST Corporation.
 * 
 * This product includes software developed by the
 * LSST Project (http://www.lsst.org/).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the LSST License Statement and 
 * the GNU General Public License along with this program.  If not, 
 * see <http://www.lsstcorp.org/LegalNotices/>.
 */
 
#ifndef LSST_AFW_TABLE_MATCH_H
#define LSST_AFW_TABLE_MATCH_H

#include <vector>

#include "lsst/afw/table/BaseRecord.h"
#include "lsst/afw/table/Vector.h"
#include "lsst/afw/geom/Angle.h"

namespace lsst { namespace afw { namespace table {

/**
 *  @brief Lightweight representation of a geometric match between two records.
 *
 *  This is a template so it can hold derived record classes without a lot of 
 *  casting and properly use Angle for the distance when we do spherical coordinate
 *  matches.
 */
template <typename Record1, typename Record2>
struct Match {
    PTR(Record1) first;
    PTR(Record2) second;
    double distance; // may be pixels or radians

    Match() : first(), second(), distance(0.0) {}
    
    Match(PTR(Record1) const & r1, PTR(Record2) const & r2, double dist)
        : first(r1), second(r2), distance(dist) {}

    template <typename R1, typename R2>
    Match(Match<R1,R2> const & other) : first(other.first), second(other.second), distance(other.distance) {}

    template <typename R1, typename R2>
    static std::vector<Match> static_vector_cast(std::vector< Match<R1,R2> > const & v) {
        std::vector<Match> r;
        r.resize(v.size());
        typename std::vector<Match>::iterator j = r.begin();
        typename std::vector< Match<R1,R2> >::const_iterator i = v.begin();
        for (; i != v.end(); ++i, ++j) {
            j->first = boost::static_pointer_cast<Record1>(i->first);
            j->second = boost::static_pointer_cast<Record2>(i->second);
            j->distance = i->distance;
        }
        return r;
    }

};

typedef Match<BaseRecord,BaseRecord> BaseMatch;

typedef std::vector<BaseMatch> BaseMatchVector;

/**
 * Compute all tuples (s1,s2,d) where s1 belings to @a set1, s2 belongs to @a set2 and
 * d, the distance between s1 and s2, in pixels, is at most @a radius. If set1 and
 * set2 are identical, then this call is equivalent to @c matchXy(set1,radius,true).
 * The match is performed in pixel space (2d cartesian).
 *
 * @param[in] set1     first set of records
 * @param[in] key1     key used to extract the center of records in set1
 * @param[in] set2     second set of records
 * @param[in] key2     key used to extract the center of records in set2
 * @param[in] radius   match radius (pixels)
 * @param[in] closest  if true then just return the closest match
 */
BaseMatchVector matchXy(
    BaseVector const & v1, Key< Point<double> > const & key1,
    BaseVector const & v2, Key< Point<double> > const & key2,
    double dist, bool closest=true
);

/**
 * Compute all tuples (s1,s2,d) where s1 != s2, s1 and s2 both belong to @a set,
 * and d, the distance between s1 and s2, in pixels, is at most @a radius. The
 * match is performed in pixel space (2d cartesian).
 *
 * @param[in] set          the set of records to self-match
 * @param[in] key          key used to extract the center
 * @param[in] radius       match radius (pixels)
 * @param[in] symmetric    if set to @c true symmetric matches are produced: i.e.
 *                         if (s1, s2, d) is reported, then so is (s2, s1, d).
 */
BaseMatchVector matchXy(
    BaseVector const & v, Key< Point<double> > const & key,
    double dist, bool symmetric=true
);

/** 
 * Compute all tuples (s1,s2,d) where s1 belings to @a set1, s2 belongs to @a set2 and
 * d, the distance between s1 and s2, is at most @a radius. If set1 and
 * set2 are identical, then this call is equivalent to @c matchRaDec(set1,radius,true).
 * The match is performed in ra, dec space.
 *
 * @param[in] set1     first set of records
 * @param[in] key1     key used to extract the center of records in set1
 * @param[in] set2     second set of records
 * @param[in] key2     key used to extract the center of records in set2
 * @param[in] radius   match radius
 * @param[in] closest  if true then just return the closest match
 */
BaseMatchVector matchRaDec(
    BaseVector const & v1, Key<Coord> const & key1,
    BaseVector const & v2, Key<Coord> const & key2,
    Angle dist, bool closest=true
);

/** 
 * Compute all tuples (s1,s2,d) where s1 != s2, s1 and s2 both belong to @a set,
 * and d, the distance between s1 and s2, is at most @a radius. The
 * match is performed in ra, dec space.
 *
 * @param[in] set          the set of records to self-match
 * @param[in] key          key used to extract the center
 * @param[in] radius       match radius
 * @param[in] symmetric    if set to @c true symmetric matches are produced: i.e.
 *                         if (s1, s2, d) is reported, then so is (s2, s1, d).
 */
BaseMatchVector matchRaDec(
    BaseVector const & v, Key<Coord> const & key,
    Angle dist, bool symmetric=true
);

/**
 *  @brief Return a table representation of a BaseMatchVector that can be used to persist it.
 *
 *  The schema of the returned object has "first" (RecordId), "second" (RecordID), and "distance"
 *  (Angle) fields.
 *
 *  @param[in]  matches     A std::vector of Match objects to convert to table form.
 *  @param[in]  idKey1      Key for the unique ID field in the Record1 schema.
 *  @param[in]  idKey2      Key for the unique ID field in the Record2 schema.
 */
BaseVector packMatches(
    BaseMatchVector const & matches,
    Key<RecordId> const & idKey1,
    Key<RecordId> const & idKey2
);

/**
 *  @brief Reconstruct a BaseMatchVector from a BaseVector representation of the matches
 *         and a pair of table VectorTs that hold the records themselves.
 *
 *  @note The table VectorT arguments must be sorted in ascending ID order on input; this will
 *        allow us to use binary search algorithms to find the records referred to by the match
 *        table.
 *
 *  If an ID cannot be found in the given tables, that pointer will be set to null
 *  in the returned match vector.
 *
 *  @param[in]  matches     A BaseTable vector representation, as produced by makeMatchTable.
 *  @param[in]  first       A VectorT containing the records used on the 'first' side of the match,
 *                          sorted by ascending ID.
 *  @param[in]  idKey1      Key for the ID key for the 'first' side of the match.
 *  @param[in]  second      A VectorT containing the records used on the 'second' side of the match,
 *                          sorted by ascending ID.  May be the same as first.
 *  @param[in]  idKey2      Key for the ID key for the 'second' side of the match.
 */
BaseMatchVector unpackMatches(
    BaseVector const & matches, 
    BaseVector const & first, Key<RecordId> const & idKey1,
    BaseVector const & second, Key<RecordId> const & idKey2
);

}}} // namespace lsst::afw::table

#endif // #ifndef LSST_AFW_TABLE_MATCH_H

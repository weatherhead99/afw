// -*- lsst-c++ -*-
#ifndef AFW_TABLE_IdFactory_h_INCLUDED
#define AFW_TABLE_IdFactory_h_INCLUDED

#include "lsst/afw/table/config.h"

#include "boost/shared_ptr.hpp"

namespace lsst { namespace afw { namespace table {

/**
 *  @brief A polymorphic functor base class for generating record IDs for a table.
 *
 *  The IDs produced by an IdFactory need not be sequential, but they must be unique and nonotonically 
 *  increasing, both with respect to the IDs it generates itself and those passed to it via the
 *  notify() member function.  Valid IDs must be nonzero, as zero is used to indicate null in some contexts.
 */
class IdFactory {
public:

    typedef boost::shared_ptr<IdFactory> Ptr;

    /// @brief Return a new unique RecordId.
    virtual RecordId operator()() = 0;

    /// @brief Notify the IdFactory that the given ID has been used and must not be returned by operator().
    virtual void notify(RecordId id) = 0;

    /// @brief Deep-copy the IdFactory.
    virtual Ptr clone() const = 0;

    static Ptr makeSimple();

    virtual ~IdFactory() {}

protected:

    /// @brief Protected to prevent slicing.
    void operator=(IdFactory const & other) {}

};

}}} // namespace lsst::afw::table

#endif // !AFW_TABLE_IdFactory_h_INCLUDED

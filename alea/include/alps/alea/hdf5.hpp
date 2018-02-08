/*
 * Copyright (C) 1998-2017 ALPS Collaboration. See COPYRIGHT.TXT
 * All rights reserved. Use is subject to license terms. See LICENSE.TXT
 * For use in publications, see ACKNOWLEDGE.TXT
 */
#pragma once

#include <sstream>
#include <iostream>
#include <numeric>

#include <alps/alea/core.hpp>

#include <alps/hdf5/archive.hpp>

namespace alps { namespace alea {

class hdf5_serializer
    : public serializer
{
public:
    hdf5_serializer(hdf5::archive &ar, const std::string &path, bool debug=false)
        : archive_(&ar)
        , path_(path)
        , group_()
        , debug_(debug)
    { }

    void enter(const std::string &group)
    {
        if (debug_)
            std::cerr << "Entering group: " << get_path(group) << "\n";
        archive_->create_group(get_path(group));  // TODO: what if exists?
        group_.push_back(group);
    }

    void exit()
    {
        if (debug_)
            std::cerr << "Exiting group: " << get_path("") << "\n";
        if (group_.empty())
            throw std::runtime_error("exit without enter");
        group_.pop_back();
    }

    void write(const std::string &key, sink<const double> value) {
        do_write(key, value);
    }

    void write(const std::string &key, sink<const std::complex<double>> value) {
        do_write(key, value);
    }

    void write(const std::string &key, sink<const complex_op<double>> value) {
        do_write(key, value);
    }

    void write(const std::string &key, sink<const long> value) {
        do_write(key, value);
    }

    void write(const std::string &key, sink<const unsigned long> value) {
        do_write(key, value);
    }

    ~hdf5_serializer()
    {
        // Cannot do exception because we are in destructor
        if (!group_.empty()) {
            std::cerr << "alps::alea::hdf5_serializer: warning: "
                      << "enter without exit\n Lingering groups:"
                      << get_path("") << "\n\n";
        }
    }

protected:
    template <typename T>
    void do_write(const std::string &relpath, sink<const T> data)
    {
        // Look at:
        // void archive::write(std::string path, T const * value,
        //     , std::vector<std::size_t> size
        //     , std::vector<std::size_t> chunk = std::vector<std::size_t>()
        //     , std::vector<std::size_t> offset = std::vector<std::size_t>()
        //     ) const;
        if (debug_)
            std::cerr << "Writing:" << get_path(relpath) << "\n";
        std::string path = get_path(relpath);

        // FIXME: support shape
        std::vector<size_t> shape = {data.size()};
        std::vector<size_t> offset(shape.size(), 0);
        std::vector<size_t> chunk = shape;

        (*archive_).write(path, data.data(), shape, chunk, offset);
    }

    std::string get_path(const std::string &key)
    {
        std::ostringstream maker(path_, std::ios_base::app);
        maker << '/';
        for (const std::string &group : group_)
            maker << group << '/';
        if (key.find('/') != std::string::npos)
            throw std::runtime_error("Key must not contain '/'");
        maker << key;
        return maker.str();
    }

private:
    hdf5::archive *archive_;
    std::string path_;
    std::vector<std::string> group_;
    bool debug_;
};

}}

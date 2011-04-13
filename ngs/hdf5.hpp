/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                 *
 * ALPS Project: Algorithms and Libraries for Physics Simulations                  *
 *                                                                                 *
 * ALPS Libraries                                                                  *
 *                                                                                 *
 * Copyright (C) 2010 - 2011 by Lukas Gamper <gamperl@gmail.com>                   *
 *                                                                                 *
 * This software is part of the ALPS libraries, published under the ALPS           *
 * Library License; you can use, redistribute it and/or modify it under            *
 * the terms of the license, either version 1 or (at your option) any later        *
 * version.                                                                        *
 *                                                                                 *
 * You should have received a copy of the ALPS Library License along with          *
 * the ALPS Libraries; see the file LICENSE.txt. If not, the license is also       *
 * available from http://alps.comp-phys.org/.                                      *
 *                                                                                 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT       *
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE       *
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,     *
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER     *
 * DEALINGS IN THE SOFTWARE.                                                       *
 *                                                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef ALPS_NGS_HDF5_HPP
#define ALPS_NGS_HDF5_HPP

#include <alps/config.h>
#include <alps/ngs/macros.hpp>

#include <boost/mpl/and.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_array.hpp>
#include <boost/type_traits/remove_all_extents.hpp>

#include <map>
#include <vector>
#include <string>
#include <numeric>

namespace alps {
    namespace hdf5 {

        namespace detail {
            struct mccontext;

            template<typename A, typename T> struct is_datatype_caller {
                static bool apply(A const & ar, std::string path) {
                    ALPS_NGS_THROW_RUNTIME_ERROR("only native datatypes can be probed: " + path)
                    return false;
                }
            };

            #define ALPS_NGS_HDF5_IS_DATATYPE_CALLER(T)                                                                                                                \
                template<typename A> struct is_datatype_caller<A, T > {                                                                                                \
                    static bool apply(A const & ar, std::string path) {                                                                                                \
                        T unused;                                                                                                                                      \
                        return ar.is_datatype_impl(path, unused);                                                                                                      \
                    }                                                                                                                                                  \
                };
            ALPS_NGS_FOREACH_NATIVE_HDF5_TYPE(ALPS_NGS_HDF5_IS_DATATYPE_CALLER)
            #undef ALPS_NGS_HDF5_IS_DATATYPE_CALLER

            struct ignore_python_destruct_errors {
                 static bool value;
            };

        }

        class ALPS_DECL archive {

            public:

                typedef enum { READ = 0x00, WRITE = 0x01, COMPRESS = 0x02 } properties;

                archive(std::string const & filename, std::size_t props = READ);
                archive(archive const & arg);

                virtual ~archive();

                std::string const & get_filename() const;

                std::string encode_segment(std::string segment) const;
                std::string decode_segment(std::string segment) const;

                std::string get_context() const;
                void set_context(std::string const & context);
                std::string complete_path(std::string path) const;

                bool is_data(std::string path) const;
                bool is_attribute(std::string path) const;
                bool is_group(std::string path) const;

                bool is_scalar(std::string path) const;
                bool is_string(std::string path) const;
                bool is_null(std::string path) const;
                bool is_complex(std::string path) const;

                template<typename T> bool is_datatype(std::string path) const {
                    return detail::is_datatype_caller<archive, T>::apply(*this, path);
                };

                std::vector<std::string> list_children(std::string path) const;
                std::vector<std::string> list_attributes(std::string path) const;

                std::vector<std::size_t> extent(std::string path) const;
                std::size_t dimensions(std::string path) const;

                void create_group(std::string path) const;

                void delete_data(std::string path) const;
                void delete_group(std::string path) const;
                void delete_attribute(std::string path) const;

                void set_complex(std::string path);

/* TODO: implement
                void move_data(std::string current_path, std::string new_path) const;
                void move_attribute(std::string current_path, std::string new_path) const;
*/

                template<typename T> void read(
                      std::string path
                    , T *
                    , std::vector<std::size_t>
                    , std::vector<std::size_t> = std::vector<std::size_t>()
                ) const {
                    ALPS_NGS_THROW_RUNTIME_ERROR("Invalid type on path: " + path)
                }

                template<typename T> void write(
                      std::string path
                    , T const * value, std::vector<std::size_t> size
                    , std::vector<std::size_t> chunk = std::vector<std::size_t>()
                    , std::vector<std::size_t> offset = std::vector<std::size_t>()
                ) const {
                    ALPS_NGS_THROW_RUNTIME_ERROR("Invalid type on path: " + path)
                }

                #define ALPS_NGS_HDF5_DEFINE_API(T)                                                                                                                    \
                    void read(std::string path, T & value) const;                                                                                                      \
                    void read(                                                                                                                                         \
                          std::string path                                                                                                                             \
                        , T * value                                                                                                                                    \
                        , std::vector<std::size_t> chunk                                                                                                               \
                        , std::vector<std::size_t> offset = std::vector<std::size_t>()                                                                                 \
                    ) const;                                                                                                                                           \
                                                                                                                                                                       \
                    void write(std::string path, T value) const;                                                                                                       \
                    void write(                                                                                                                                        \
                          std::string path                                                                                                                             \
                        , T const * value, std::vector<std::size_t> size                                                                                               \
                        , std::vector<std::size_t> chunk = std::vector<std::size_t>()                                                                                  \
                        , std::vector<std::size_t> offset = std::vector<std::size_t>()                                                                                 \
                    ) const;
                ALPS_NGS_FOREACH_NATIVE_HDF5_TYPE(ALPS_NGS_HDF5_DEFINE_API)
                #undef ALPS_NGS_HDF5_DEFINE_API

                #define ALPS_NGS_HDF5_IS_DATATYPE_IMPL_DECL(T)                                                                                                         \
                    bool is_datatype_impl(std::string path, T) const;
                ALPS_NGS_FOREACH_NATIVE_HDF5_TYPE(ALPS_NGS_HDF5_IS_DATATYPE_IMPL_DECL)
                #undef ALPS_NGS_HDF5_IS_DATATYPE_IMPL_DECL

            private:

                std::string file_key(std::string filename, bool writeable, bool compressed) const;

                std::string current_;
                detail::mccontext * context_;
                static std::map<std::string, boost::weak_ptr<detail::mccontext> > _pool;

                static std::map<std::string, std::pair<detail::mccontext *, std::size_t> > ref_cnt_;

        };

        template<typename T> struct is_continous
            : public boost::false_type
        {};

        template<typename T> struct has_complex_elements
            : public boost::false_type
        {};
        
        template<typename T> struct scalar_type {
            typedef T type;
        };

        namespace detail {
                
            template<typename T> struct remove_cvr {
                typedef T type;
            };
        
            template<typename T> struct remove_cvr<T const> {
                typedef typename remove_cvr<T>::type type;
            };
        
            template<typename T> struct remove_cvr<T volatile> {
                typedef typename remove_cvr<T>::type type;
            };
        
            template<typename T> struct remove_cvr<T &> {
                typedef typename remove_cvr<T>::type type;
            };

             template<typename T> struct get_extent {
                static std::vector<std::size_t> apply(T const & value) {
                    return std::vector<std::size_t>();
                }
            };

            template<typename T> struct set_extent {
                 static void apply(T &, std::vector<std::size_t> const &) {}
            };

            template<typename T> struct is_vectorizable {
                 static bool apply(T const & value){
                    return false;
                }
            };

            template<typename T> struct get_pointer {
                 static typename alps::hdf5::scalar_type<T>::type * apply(T &) {
                    return NULL;
                }
            };

            template<typename T> struct get_pointer<T const> {
                 static typename alps::hdf5::scalar_type<T>::type const * apply(T const &) {
                    return NULL;
                }
            };

        }

         template<typename T> typename scalar_type<T>::type * get_pointer(T & value) {
            return detail::get_pointer<T>::apply(value);
        }

        template<typename T> typename scalar_type<T>::type const * get_pointer(T const & value) {
            return detail::get_pointer<T const>::apply(value);
        }

        template<typename T> std::vector<std::size_t> get_extent(T const & value) {
            return detail::get_extent<T>::apply(value);
        }

        template<typename T> void set_extent(T & value, std::vector<std::size_t> const & size) {
            detail::set_extent<T>::apply(value, size);
        }

        template<typename T> bool is_vectorizable(T const & value) {
            return detail::is_vectorizable<T>::apply(value);
        }

        template<typename T> void save(
               archive & ar
             , std::string const & path
             , T const & value
             , std::vector<std::size_t> size = std::vector<std::size_t>()
             , std::vector<std::size_t> chunk = std::vector<std::size_t>()
             , std::vector<std::size_t> offset = std::vector<std::size_t>()
        ) {
            if (chunk.size())
                ALPS_NGS_THROW_RUNTIME_ERROR("user defined objects needs to be written continously");
            std::string context = ar.get_context();
            ar.set_context(ar.complete_path(path));
            value.save(ar);
            ar.set_context(context);
        }

        template<typename T> void load(
               archive & ar
             , std::string const & path
             , T & value
             , std::vector<std::size_t> chunk = std::vector<std::size_t>()
             , std::vector<std::size_t> offset = std::vector<std::size_t>()
        ) {
            if (chunk.size())
                ALPS_NGS_THROW_RUNTIME_ERROR("user defined objects needs to be written continously");
            std::string context = ar.get_context();
            ar.set_context(ar.complete_path(path));
            value.load(ar);
            ar.set_context(context);
        }

        #define ALPS_NGS_HDF5_DEFINE_FREE_FUNCTIONS(T)                                                                                                                 \
            template<> struct is_continous< T >                                                                                                                        \
                : public boost::true_type                                                                                                                              \
            {};                                                                                                                                                        \
                                                                                                                                                                       \
            namespace detail {                                                                                                                                         \
                template<> struct ALPS_DECL is_vectorizable< T > {                                                                                                               \
                    static bool apply(T const & value);                                                                                                                \
                };                                                                                                                                                     \
                                                                                                                                                                       \
                template<> struct ALPS_DECL get_pointer< T > {                                                                                                                   \
                    static alps::hdf5::scalar_type< T >::type * apply( T & value);                                                                                     \
                };                                                                                                                                                     \
                                                                                                                                                                       \
                template<> struct ALPS_DECL get_pointer< T const > {                                                                                                             \
                    static alps::hdf5::scalar_type< T >::type const * apply( T const & value);                                                                         \
                };                                                                                                                                                     \
            }                                                                                                                                                          \
                                                                                                                                                                       \
            ALPS_DECL void save(                                                                                                                                                 \
                  archive & ar                                                                                                                                         \
                , std::string const & path                                                                                                                             \
                , T const & value                                                                                                                                      \
                , std::vector<std::size_t> size = std::vector<std::size_t>()                                                                                           \
                , std::vector<std::size_t> chunk = std::vector<std::size_t>()                                                                                          \
                , std::vector<std::size_t> offset = std::vector<std::size_t>()                                                                                         \
            );                                                                                                                                                         \
                                                                                                                                                                       \
            ALPS_DECL void load(                                                                                                                                                 \
                  archive & ar                                                                                                                                         \
                , std::string const & path                                                                                                                             \
                , T & value                                                                                                                                            \
                , std::vector<std::size_t> chunk = std::vector<std::size_t>()                                                                                          \
                , std::vector<std::size_t> offset = std::vector<std::size_t>()                                                                                         \
            );
        ALPS_NGS_FOREACH_NATIVE_HDF5_TYPE(ALPS_NGS_HDF5_DEFINE_FREE_FUNCTIONS)
        #undef ALPS_NGS_HDF5_DEFINE_FREE_FUNCTIONS

        namespace detail {

            template<typename T> struct make_pvp_proxy {

                public:

                    explicit make_pvp_proxy(std::string const & path, T value)
                        : path_(path), value_(value)
                    {}

                    make_pvp_proxy(make_pvp_proxy<T> const & arg)
                        : path_(arg.path_), value_(arg.value_) 
                    {}

                    std::string path_;
                    T value_;
            };

        }

        template <typename T> typename boost::enable_if<
              has_complex_elements<typename detail::remove_cvr<T>::type>
            , archive &
        >::type operator<< (archive & ar, detail::make_pvp_proxy<T> const & proxy) {
            save(ar, proxy.path_, proxy.value_);
            ar.set_complex(proxy.path_);
            return ar;
        }

        template <typename T> typename boost::disable_if<
              has_complex_elements<typename detail::remove_cvr<T>::type>
            , archive &
        >::type operator<< (archive & ar, detail::make_pvp_proxy<T> const & proxy) {
            save(ar, proxy.path_, proxy.value_);
            return ar;
        }

        template <typename T> archive & operator>> (archive & ar, detail::make_pvp_proxy<T> proxy) {
            load(ar, proxy.path_, proxy.value_);
            return ar;
        }
    }

    template <typename T> typename boost::disable_if<typename boost::mpl::and_<
          typename boost::is_same<typename hdf5::detail::remove_cvr<typename boost::remove_all_extents<T>::type>::type, char>::type
        , typename boost::is_array<T>::type
    >::type, hdf5::detail::make_pvp_proxy<T &> >::type make_pvp(std::string const & path, T & value) {
        return hdf5::detail::make_pvp_proxy<T &>(path, value);
    }
    template <typename T> typename boost::disable_if<typename boost::mpl::and_<
          typename boost::is_same<typename hdf5::detail::remove_cvr<typename boost::remove_all_extents<T>::type>::type, char>::type
        , typename boost::is_array<T>::type
    >::type, hdf5::detail::make_pvp_proxy<T const &> >::type make_pvp(std::string const & path, T const & value) {
        return hdf5::detail::make_pvp_proxy<T const &>(path, value);
    }
    template <typename T> typename boost::enable_if<typename boost::mpl::and_<
          typename boost::is_same<typename hdf5::detail::remove_cvr<typename boost::remove_all_extents<T>::type>::type, char>::type
        , typename boost::is_array<T>::type
    >::type, hdf5::detail::make_pvp_proxy<std::string const> >::type make_pvp(std::string const & path, T const & value) {
        return hdf5::detail::make_pvp_proxy<std::string const>(path, value);
    }

}

#endif
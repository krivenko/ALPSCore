/** Serialization tests */

/*
 * Copyright (C) 1998-2014 ALPS Collaboration. See COPYRIGHT.TXT
 * All rights reserved. Use is subject to license terms. See LICENSE.TXT
 * For use in publications, see ACKNOWLEDGE.TXT
 */

#include <iostream>
#include <sstream>

// Serialization headers:
#include "boost/archive/text_oarchive.hpp"
#include "boost/archive/text_iarchive.hpp"

#include "alps/utilities/temporary_filename.hpp"
#include <alps/hdf5/archive.hpp>

#include "alps/params.hpp"
#include "gtest/gtest.h"

// void Test()
TEST(param, Serialization)
{
    const char* argv[]={ "", "--param1=111" };
    const int argc=sizeof(argv)/sizeof(*argv);
    alps::params p(argc,argv);

    p.description("Serialization test").
        define<int>("param1","integer 1").
        define<double>("param2",22.25,"double");
    p["param3"]=333;
           

    std::ostringstream outs; 
    {
        boost::archive::text_oarchive ar(outs);
        ar << p;
    }

    // std::cerr << outs.str();

    alps::params p2;
    std::istringstream ins(outs.str());
    {
        boost::archive::text_iarchive ar(ins);
        ar >> p2;
    }

    EXPECT_EQ(111, p2["param1"]);
    EXPECT_EQ(22.25, p2["param2"]);
    EXPECT_EQ(333, p2["param3"]);
}

TEST(param, Archive)
{
    // Prepare parameters
    const char* argv[]={ "", "--param1=111" };
    const int argc=sizeof(argv)/sizeof(*argv);
    alps::params p(argc,argv);

    p.description("Archiving test").
        define<int>("param1","integer 1").
        define<double>("param2",22.25,"double");
    p["param3"]=333;

    // Save to archive
    std::string filename(alps::temporary_filename("hdf5_file"));
    {
        alps::hdf5::archive oar(filename, "w");
        p.save(oar);
    }

    // Load from archive
    alps::params p2;
    {
        alps::hdf5::archive iar(filename, "r");
        p2.load(iar);
    }
    EXPECT_EQ(111, p2["param1"]);
    EXPECT_EQ(22.25, p2["param2"]);
    EXPECT_EQ(333, p2["param3"]);

    // Create from archive
    alps::hdf5::archive iar3(filename, "r");
    alps::params p3(iar3, "/");
    EXPECT_EQ(111, p3["param1"]);
    EXPECT_EQ(22.25, p3["param2"]);
    EXPECT_EQ(333, p3["param3"]);
}

int main(int argc, char** argv)
{
    // return Test(), 0;
    
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

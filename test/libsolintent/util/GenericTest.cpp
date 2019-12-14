/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Tests for libsolintent/util/Generic.cpp.
 */

#include <libsolintent/util/Generic.h>

#include <test/CompilerFramework.h>
#include <boost/test/unit_test.hpp>

using namespace std;

namespace dev
{
namespace solintent
{
namespace test
{

BOOST_AUTO_TEST_SUITE(GenreicTest)

BOOST_AUTO_TEST_CASE(scoped_sets)
{
    const int V1 = 5;
    const int V2 = 10;
    const int V3 = 15;
    const int V4 = 20;
    const int V5 = 25;
    const int V6 = 30;
    const int V7 = 35;

    int v = V1;
    {
        ScopedSet<int> s1(v, V2);
        BOOST_CHECK_EQUAL(v, V2);
        BOOST_CHECK_EQUAL(s1.old(), V1);
        {
            ScopedSet<int> s2(v, V3);
            BOOST_CHECK_EQUAL(v, V3);
            BOOST_CHECK_EQUAL(s2.old(), V2);
            {
                ScopedSet<int> s3(v, V4);
                BOOST_CHECK_EQUAL(v, V4);
                BOOST_CHECK_EQUAL(s3.old(), V3);
            }
            BOOST_CHECK_EQUAL(v, V3);
        }
        BOOST_CHECK_EQUAL(v, V2);

        ScopedSet<int> s4(v, V5);
        BOOST_CHECK_EQUAL(v, V5);
        BOOST_CHECK_EQUAL(s4.old(), V2);
        {
            ScopedSet<int> s5(v, V6);
            BOOST_CHECK_EQUAL(v, V6);
            BOOST_CHECK_EQUAL(s5.old(), V5);
            {
                ScopedSet<int> s6(v, V7);
                BOOST_CHECK_EQUAL(v, V7);
                BOOST_CHECK_EQUAL(s6.old(), V6);
            }
            BOOST_CHECK_EQUAL(v, V6);
        }
        BOOST_CHECK_EQUAL(v, V5);
    }
    BOOST_CHECK_EQUAL(v, V1);
}

BOOST_AUTO_TEST_SUITE_END()

}
}
}

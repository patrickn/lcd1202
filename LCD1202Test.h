
#ifndef __LCD1202Test_H__
#define __LCD1202Test_H__


#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>


class LCD1202Test : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(LCD1202Test);
    CPPUNIT_TEST(test1);
    CPPUNIT_TEST(test2);
    CPPUNIT_TEST(test3);
    CPPUNIT_TEST(test4);
    CPPUNIT_TEST_SUITE_END();

    public:
        void setUp(void);
        void tearDown(void);

    protected:
        void test1(void);
        void test2(void);
        void test3(void);
        void test4(void);

    private:

};


#endif

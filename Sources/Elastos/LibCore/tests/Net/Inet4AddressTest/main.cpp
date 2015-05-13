#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <elautoptr.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::test_isMulticastAddress, // 1.ok
    &CTest::test_isAnyLocalAddress, // 2.ok
    &CTest::test_isLoopbackAddress, // 3.ok
    &CTest::test_isLinkLocalAddress, // 4.ok
    &CTest::test_isSiteLocalAddress, // 5.ok
    &CTest::test_isMCGlobal, // 6.ok
    &CTest::test_isMCNodeLocal, // 7.ok
    &CTest::test_isMCLinkLocal, // 8.ok
    &CTest::test_isMCSiteLocal, // 9.ok
    &CTest::test_isMCOrgLocal, // 10.ok
};

int main(int argc, char *argv[])
{
    if (argc == 1) {
        printf("*ERROR* Require test case number\n");
        return -1;
    }

    int nIndex = atoi(argv[1]) - 1;
    int nLength = sizeof(TestEntry) / sizeof(TestEntry[0]);

    if (nIndex < 0 || nIndex >= nLength) {
        printf("*ERROR* Invalid testing case number\n");
        return -1;
    }

    CTest TestObject;

    int nQuitCode = (TestObject.*TestEntry[nIndex])(argc, argv);

    return nQuitCode;
}

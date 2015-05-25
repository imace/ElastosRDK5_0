
#include <elautoptr.h>
#include <Reflection.h>

using namespace Elastos;
using namespace Reflection;

void testGetSet()
{
    PEL("======== testGetSet() ======")

    AutoPtr<IFooBar> test;
    CFooBar::New((IFooBar**)&test);
    assert(test != NULL);

    test->SetValue(10);
    test->SetValue(String("Elastos"));

    Int32 value0;
    test->GetValue(&value0);
    String strValue;
    test->GetValue(&strValue);

    PFL_EX(" >> value0:%d, String:[%s]", value0, strValue.string());
}

ECode testReflection()
{
    printf("\n");
    PEL("======== testReflection() ======")

    AutoPtr<IModuleInfo> moduleInfo;
    // AutoPtr<IEnumInfo> enumInfo;
    // AutoPtr<IEnumItemInfo> emumItemInfo;
    AutoPtr<IClassInfo> classInfo;
    AutoPtr<IInterface> object;
    AutoPtr<IMethodInfo> methodInfo;
    AutoPtr<IArgumentList> argumentList;

    const String moduleName("/data/data/com.elastos.runtime/elastos/Reflection.eco");
    const String klassName("CFooBar");

    ECode ec = _CReflector_AcquireModuleInfo(moduleName, (IModuleInfo**)&moduleInfo);
    if (FAILED(ec)) {
        printf("Acquire \"%s\" module info failed!\n", moduleName.string());
        return ec;
    }

    ec = moduleInfo->GetClassInfo(klassName, (IClassInfo**)&classInfo);
    if (FAILED(ec)) {
        printf("Acquire \"%s\" class info failed!\n", klassName.string());
        return ec;
    }

    // Create object
    //
    ec = classInfo->CreateObject((IInterface**)&object);
    if (FAILED(ec)) {
        printf("Create object failed!\n");
        return ec;
    }

    // SetValue
    //
    ec = classInfo->GetMethodInfo("SetValue", (IMethodInfo**)&methodInfo);
    if (FAILED(ec)) {
        printf("Acquire \"SetValue\" method info failed!\n");
        return ec;
    }

    ec = methodInfo->CreateArgumentList((IArgumentList**)&argumentList);
    if (FAILED(ec)) {
        printf("Acquire \"SetValue\" method info failed!\n");
        return ec;
    }

    String strValue("Elastos");
    argumentList->SetInputArgumentOfString(0, strValue);

    ec = methodInfo->Invoke(object, argumentList);
    if (FAILED(ec)) {
        printf("Invoke \"SetValue\" method failed!\n");
        return ec;
    }

    // GetValue
    //
    methodInfo = NULL;
    ec = classInfo->GetMethodInfo("GetValue", (IMethodInfo**)&methodInfo);
    if (FAILED(ec)) {
        printf("Acquire \"GetValue\" method info failed!\n");
        return ec;
    }

    argumentList = NULL;
    ec = methodInfo->CreateArgumentList((IArgumentList**)&argumentList);
    if (FAILED(ec)) {
        printf("Acquire \"GetValue\" method info failed!\n");
        return ec;
    }

    String result;
    argumentList->SetOutputArgumentOfStringPtr(0, &result);

    ec = methodInfo->Invoke(object, argumentList);
    if (FAILED(ec)) {
        printf("Invoke \"GetValue\" method failed!\n");
        return ec;
    }

    printf(" >>> GetValue: [%s]\n", result.string());

    // Validate
    //
    IFooBar* foobar = IFooBar::Probe(object.Get());
    assert(foobar != NULL);
    String respect;
    foobar->GetValue(&respect);

    assert(respect == result);

    // SetValue
    //
    methodInfo = NULL;
    ec = classInfo->GetMethodInfo("SetValue", (IMethodInfo**)&methodInfo);
    if (FAILED(ec)) {
        printf("Acquire \"SetValue\" method info failed!\n");
        return ec;
    }

    argumentList = NULL;
    ec = methodInfo->CreateArgumentList((IArgumentList**)&argumentList);
    if (FAILED(ec)) {
        printf("Acquire \"SetValue\" method info failed!\n");
        return ec;
    }

    strValue = String("-Elastos-");
    argumentList->SetInputArgumentOfInt32(0, 11);
    argumentList->SetInputArgumentOfString(1, strValue);
    argumentList->SetInputArgumentOfInt64(2, 22);

    ec = methodInfo->Invoke(object, argumentList);
    if (FAILED(ec)) {
        printf("Invoke \"SetValue\" method failed!\n");
        return ec;
    }

    // GetValue
    //
    methodInfo = NULL;
    ec = classInfo->GetMethodInfo("GetValue", (IMethodInfo**)&methodInfo);
    if (FAILED(ec)) {
        printf("Acquire \"GetValue\" method info failed!\n");
        return ec;
    }

    argumentList = NULL;
    ec = methodInfo->CreateArgumentList((IArgumentList**)&argumentList);
    if (FAILED(ec)) {
        printf("Acquire \"GetValue\" method info failed!\n");
        return ec;
    }

    String strResult;
    Int32 value0;
    Int64 value1;

    argumentList->SetOutputArgumentOfInt32Ptr(0, &value0);
    argumentList->SetOutputArgumentOfStringPtr(1, &strResult);
    argumentList->SetOutputArgumentOfInt64Ptr(2, &value1);

    ec = methodInfo->Invoke(object, argumentList);
    if (FAILED(ec)) {
        printf("Invoke \"GetValue\" method failed!\n");
        return ec;
    }

    printf(" >>> GetValue: [%s], value0: %d, value1: %lld\n",
        strResult.string(), value0, value1);
    return NOERROR;
}

int main(int argc, char *argv[])
{
    testGetSet();

    testReflection();
    return 0;
}

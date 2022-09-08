#include <iostream>
#include <string>
#include "MathTest/MathTest.h"
#include "Test/UtilityTest.h"
#include "Templates/UtilityTemplate.h"
#include "Delegates/Delegate.h"
#include <vector>
#include "PlaceNew/PlaceNewTest.h"
#include <windows.h>

using namespace std;


class FTestClass1
{
public:
	int32 A = 1;
};


class FTestClass2
{
public:
	int8 a = 1;
	int32 B = 2;
};

class FTestClass3
{
public:
	int8 A = 1;
	int32 B = 2;
	int32 C = 2;
};


struct Test_TDelegateHandleBase
{
	typedef void (*FuncType)();

	struct FFunctionInfo
	{
		FFunctionInfo(FuncType InFunction, const char* InFuncName)
			: pFunction(InFunction)
			, FuncName(InFuncName)
		{

		}
		FuncType pFunction = nullptr;
		const char *FuncName = nullptr;
	};

	Test_TDelegateHandleBase()
	{
#define AddFunc(FunctionName) FunctionList.push_back({&Test_TDelegateHandleBase::FunctionName, #FunctionName})
		AddFunc(Test1);
		AddFunc(Test2);
		AddFunc(Test3);
		AddFunc(Test4);
		AddFunc(Test5);
#undef AddFunc
	}

	std::vector<FFunctionInfo> FunctionList;

	static void Test1()
	{
		FTestClass1 TestClass1;
		FTestClass2 TestClass2;
		auto DelegateHandle = TDelegateHandleBase<void, FTestClass1>::CreateLambda(
			[](FTestClass1 p1, FTestClass2 p2)
			{
				printf("%d %d %d\n", p1.A, p2.a, p2.B);
			}, TestClass2);

		DelegateHandle.Execute(TestClass1);
	}

	static void Test2()
	{
		FTestClass1 TestClass1;
		FTestClass2 TestClass2;
		auto DelegateHandle = TDelegateHandleBase<void, FTestClass1, int32>::CreateLambda(
			[](FTestClass1 p1, int32 InParam, FTestClass2 p2)
			{
				printf("%d %d %d %d\n", p1.A, InParam, p2.a, p2.B);
			}, TestClass2);

		DelegateHandle.Execute(TestClass1, 8);
	}
	

	/**
	 * test for return
	 */
	static void Test3()
	{
		FTestClass1 TestClass1;
		FTestClass2 TestClass2;
		auto DelegateHandle = TDelegateHandleBase<FTestClass3, FTestClass1, int32>::CreateLambda(
			[](FTestClass1 p1, int32 InParam, FTestClass2 p2)
			{
				printf("%d %d %d %d\n", p1.A, InParam, p2.a, p2.B);
				FTestClass3 TestClass3;
				TestClass3.A = p2.a;
				TestClass3.B = InParam;
				TestClass3.C = p2.B;

				return TestClass3;
			}, TestClass2);

		FTestClass3 TestClass3 = DelegateHandle.Execute(TestClass1, 8);
		printf("Return---%d %d %d\n", TestClass3.A, TestClass3.B, TestClass3.C);
	}

	static void Test4()
	{
		class TestStaticDelegateInstance
		{
		public:
			static int32 DoTest(const FTestClass1 InA, const FTestClass2 InB)
			{
				printf("TestStaticDelegateInstance::DoTest---InA:%d,InB:%d\n", InA.A, InB.a);
				return InA.A+InB.a;
			}
		};
		
		FTestClass1 TestClass1;
		FTestClass2 TestClass2;
		auto DelegateHandle = TDelegateHandleBase<int32, FTestClass1>::CreateStatic(&TestStaticDelegateInstance::DoTest, TestClass2);
		
		int32 Ret = DelegateHandle.Execute(TestClass1);
		printf("Return---Ret:%d\n", Ret);
	}

	static void Test5()
	{
		class FTestRawMethodDelegateInstance
		{
		public:
			int32 DoTest( const FTestClass1 InA, const FTestClass2 InB) const
			{
				printf("TestStaticDelegateInstance::DoTest---InA:%d,InB:%d\n", InA.A, InB.a);
				return InA.A + InB.a;
			}
		};

		FTestRawMethodDelegateInstance TestRawMethodDelegateInstance;
		FTestClass1 TestClass1;
		FTestClass2 TestClass2;
		TestClass1.A = 80;
		TestClass2.a = 90;


		auto DelegateHandle = TDelegateHandleBase<int32, FTestClass1>::CreateRaw(&TestRawMethodDelegateInstance, &FTestRawMethodDelegateInstance::DoTest, TestClass2);
		TDelegateHandleBase<int32, FTestClass1> DelegateHandle1;
		DelegateHandle1 = MoveTemp(DelegateHandle);
		int32 Ret = DelegateHandle1.Execute(TestClass1);
		printf("Return---Ret:%d\n", Ret);
	}

	void Dotest()
	{
		for (const FFunctionInfo &FunctionInfo: FunctionList)
		{
			printf("-------------------%s--------------------------\n", FunctionInfo.FuncName);
			(*FunctionInfo.pFunction)();
			printf("\n\n\n");
		}
	}
};

void TestPlaceNew()
{
	FPlaceNewTest PlaceNewTest;
	FPlaceNewTest &PlaceNewTestRef = PlaceNewTest;

	class FPlaceNewTestTemp
	{
	public:
		FPlaceNewTestTemp(int InA)
			: a(InA)
		{

		}
		int a = 1;
	};




	new (PlaceNewTestRef) FPlaceNewTestTemp(10);
}

void TestUUID()
{
	// 定义结构体
	struct s
	{
		int i;
	};

	// 为该结构体关联指定GUID
	struct __declspec(uuid("93A1665E-C9FA-4147-AC3A-3CC855281AF8")) s;

	// 获取GUID的测试代码:
	s a, *b;
	s &c = a;
	__uuidof(s);
	__uuidof(a);
	__uuidof(b);
	__uuidof(c);

	{
		LPOLESTR lpolestr;
		StringFromCLSID(__uuidof(s), &lpolestr);
		wprintf_s(L"UUID for struct:%s\n", lpolestr);
		CoTaskMemFree(lpolestr);
	}

	{
		LPOLESTR lpolestr;
		StringFromCLSID(__uuidof(a), &lpolestr);
		wprintf_s(L"UUID for variable:%s\n", lpolestr);
		CoTaskMemFree(lpolestr);
	}

	{
		LPOLESTR lpolestr;
		StringFromCLSID(__uuidof(b), &lpolestr);
		wprintf_s(L"UUID for point:%s\n", lpolestr);
		CoTaskMemFree(lpolestr);
	}

	{
		LPOLESTR lpolestr;
		StringFromCLSID(__uuidof(c), &lpolestr);
		wprintf_s(L"UUID for reference:%s\n", lpolestr);
		CoTaskMemFree(lpolestr);
	}

	printf("\n\n\n");

}


int main()
{
	TestUUID();
	TestPlaceNew();
	//	MathTest();
	// 	UtiltiyTest();
	// 
	// 	FCoreTest CoreTest;
	// 	CoreTest.TestMemberFunction();
	// 	CoreTest.TestStaticFunction();

	Test_TDelegateHandleBase Test;
	Test.Dotest();

	return 0;
}



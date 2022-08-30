#include <iostream>
#include <string>
#include "MathTest/MathTest.h"
#include "Test/UtilityTest.h"
#include "Templates/UtilityTemplate.h"
#include "Delegates/Delegate.h"

using namespace std;

struct Test_TDelegateHandleBase
{
	static void Test()
	{
		TDelegateHandleBase<void, int32> DelegateHandle = TDelegateHandleBase<void, int32>::CreateLambda(
			[](int32 A)
			{
				printf("%d", A);
			});

		DelegateHandle.Execute(15);
	}
	
};


int main()
{
	//	MathTest();
	// 	UtiltiyTest();
	// 
	// 	FCoreTest CoreTest;
	// 	CoreTest.TestMemberFunction();
	// 	CoreTest.TestStaticFunction();

	Test_TDelegateHandleBase::Test();

	return 0;
}



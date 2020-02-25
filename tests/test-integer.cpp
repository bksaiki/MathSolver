#include <iostream>
#include "../src/test/test-common.h"
#include "../src/types/Integer.h"

using namespace MathSolver;

#define TEST_COUNT 10

int main()
{
	Integer cints[TEST_COUNT];

	std::string strs[TEST_COUNT] = {
		"12351235213512351351235123512352344",
		"-9590845098800818953801280582435",
		"9243689238946312348234",
		"-1234123434620462340",
		"648623046",
		"-2342341323",
		"43214",
		"-353451",
		"25",
		"-149"
	};

	int nums[TEST_COUNT] = {
		INT32_MAX,
		INT32_MIN,
		874340509,
		-130453055,
		9235123,
		-6135355,
		43214,
		-353451,
		25,
		-149
	};

	bool status = true;

	//
	//	Tests
	//

	TestModule tests("Constructor (int)");
	for (size_t i = 0; i < TEST_COUNT; ++i)
	{
		Integer tmp(nums[i]);
		tests.runTest(tmp.toString(), std::to_string(nums[i]));
	}
	std::cout << tests.result() << std::endl;
	status &= tests.status();
	
	tests.reset("Constructor (std::string)");
	for (size_t i = 0; i < TEST_COUNT; ++i)
	{
		Integer tmp(strs[i]);
		tests.runTest(tmp.toString(), strs[i]);
	}
	std::cout << tests.result() << std::endl;
	status &= tests.status();
	
	tests.reset("Constructor (C-string)");
	for (size_t i = 0; i < TEST_COUNT; ++i)
	{
		Integer tmp(strs[i].c_str());
		tests.runTest(tmp.toString(), strs[i]);
	}
	std::cout << tests.result() << std::endl;
	status &= tests.status();
	
	tests.reset("Assignment (int)");
	for (size_t i = 0; i < TEST_COUNT; ++i)
	{
		cints[i] = nums[i];
		tests.runTest(cints[i].toString(), std::to_string(nums[i]));
	}
	std::cout << tests.result() << std::endl;
	status &= tests.status();
	
	tests.reset("Assignment (std::string)");
	for (size_t i = 0; i < TEST_COUNT; ++i)
	{
		cints[i] = strs[i];
		tests.runTest(cints[i].toString(), strs[i]);
	}
	std::cout << tests.result() << std::endl;
	status &= tests.status();
	
	tests.reset("Assignment (C-string)");
	for (size_t i = 0; i < TEST_COUNT; ++i)
	{
		cints[i] = strs[i].c_str();
		tests.runTest(cints[i].toString(), strs[i]);
	}
	std::cout << tests.result() << std::endl;
	status &= tests.status();

	{		
		tests.reset("a+b");
	
		Integer r01 = cints[0] + cints[1];
		Integer r23 = cints[2] + cints[3];
		Integer r45 = cints[4] + cints[5];
		Integer r67 = cints[6] + cints[7];
		Integer r89 = cints[8] + cints[9];
		Integer r13 = cints[1] + cints[3];
		Integer r25 = cints[2] + cints[5];
		Integer r49 = cints[4] + cints[9];
		Integer r68 = cints[6] + cints[8];
		Integer r07 = cints[0] + cints[7];

		tests.runTest(r01.toString(), "12341644368413550532281322231769909");
		tests.runTest(r23.toString(), "9242455115511691885894");
		tests.runTest(r45.toString(), "-1693718277");
		tests.runTest(r67.toString(), "-310237");
		tests.runTest(r89.toString(), "-124");
		tests.runTest(r13.toString(), "-9590845098802053077235901044775");
		tests.runTest(r25.toString(), "9243689238943970006911");
		tests.runTest(r49.toString(), "648622897");
		tests.runTest(r68.toString(), "43239");
		tests.runTest(r07.toString(), "12351235213512351351235123511998893");

		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	{		
		tests.reset("a+=b");

		Integer r01 = cints[0];
		Integer r23 = cints[2];
		Integer r45 = cints[4];
		Integer r67 = cints[6];
		Integer r89 = cints[8];
		Integer r13 = cints[1];
		Integer r25 = cints[2];
		Integer r49 = cints[4];
		Integer r68 = cints[6];
		Integer r07 = cints[0];

		r01 += cints[1];
		r23+= cints[3];
		r45 += cints[5];
		r67 += cints[7];
		r89 += cints[9];
		r13 += cints[3];
		r25 += cints[5];
		r49 += cints[9];
		r68 += cints[8];
		r07 += cints[7];

		tests.runTest(r01.toString(), "12341644368413550532281322231769909");
		tests.runTest(r23.toString(), "9242455115511691885894");
		tests.runTest(r45.toString(), "-1693718277");
		tests.runTest(r67.toString(), "-310237");
		tests.runTest(r89.toString(), "-124");
		tests.runTest(r13.toString(), "-9590845098802053077235901044775");
		tests.runTest(r25.toString(), "9243689238943970006911");
		tests.runTest(r49.toString(), "648622897");
		tests.runTest(r68.toString(), "43239");
		tests.runTest(r07.toString(), "12351235213512351351235123511998893");

		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	{		
		tests.reset("a-b");

		Integer r01 = cints[0] - cints[1];
		Integer r23 = cints[2] - cints[3];
		Integer r45 = cints[4] - cints[5];
		Integer r67 = cints[6] - cints[7];
		Integer r89 = cints[8] - cints[9];
		Integer r13 = cints[1] - cints[3];
		Integer r25 = cints[2] - cints[5];
		Integer r49 = cints[4] - cints[9];
		Integer r68 = cints[6] - cints[8];
		Integer r07 = cints[0] - cints[7];

		tests.runTest(r01.toString(), "12360826058611152170188924792934779");
		tests.runTest(r23.toString(), "9244923362380932810574");
		tests.runTest(r45.toString(), "2990964369");
		tests.runTest(r67.toString(), "396665");
		tests.runTest(r89.toString(), "174");
		tests.runTest(r13.toString(), "-9590845098799584830366660120095");
		tests.runTest(r25.toString(), "9243689238948654689557");
		tests.runTest(r49.toString(), "648623195");
		tests.runTest(r68.toString(), "43189");
		tests.runTest(r07.toString(), "12351235213512351351235123512705795");

		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	{		
		tests.reset("a-=b");

		Integer r01 = cints[0];
		Integer r23 = cints[2];
		Integer r45 = cints[4];
		Integer r67 = cints[6];
		Integer r89 = cints[8];
		Integer r13 = cints[1];
		Integer r25 = cints[2];
		Integer r49 = cints[4];
		Integer r68 = cints[6];
		Integer r07 = cints[0];

		r01 -= cints[1];
		r23 -= cints[3];
		r45 -= cints[5];
		r67 -= cints[7];
		r89 -= cints[9];
		r13 -= cints[3];
		r25 -= cints[5];
		r49 -= cints[9];
		r68 -= cints[8];
		r07 -= cints[7];

		tests.runTest(r01.toString(), "12360826058611152170188924792934779");
		tests.runTest(r23.toString(), "9244923362380932810574");
		tests.runTest(r45.toString(), "2990964369");
		tests.runTest(r67.toString(), "396665");
		tests.runTest(r89.toString(), "174");
		tests.runTest(r13.toString(), "-9590845098799584830366660120095");
		tests.runTest(r25.toString(), "9243689238948654689557");
		tests.runTest(r49.toString(), "648623195");
		tests.runTest(r68.toString(), "43189");
		tests.runTest(r07.toString(), "12351235213512351351235123512705795");

		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	{	
		tests.reset("a*b");

		Integer r01 = cints[0] * cints[1];
		Integer r23 = cints[2] * cints[3];
		Integer r45 = cints[4] * cints[5];
		Integer r67 = cints[6] * cints[7];
		Integer r89 = cints[8] * cints[9];
		Integer r13 = cints[1] * cints[3];
		Integer r25 = cints[2] * cints[5];
		Integer r49 = cints[4] * cints[9];
		Integer r68 = cints[6] * cints[8];
		Integer r07 = cints[0] * cints[7];

		tests.runTest(r01.toString(), "-118458783711651021581285558191208126241981437713782681117257477640");
		tests.runTest(r23.toString(), "-11407853512132630592268596009512162507560");
		tests.runTest(r45.toString(), "-1519296563695929858");
		tests.runTest(r67.toString(), "-15274031514");
		tests.runTest(r89.toString(), "-3725");
		tests.runTest(r13.toString(), "11836286694244894161857011067984096726964182997900");
		tests.runTest(r25.toString(), "-21651875281354368391733664273582");
		tests.runTest(r49.toString(), "-96644833854");
		tests.runTest(r68.toString(), "1080350");
		tests.runTest(r07.toString(), "-4365556437451154097445405640564448339144");

		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	{		
		tests.reset("a*=b");

		Integer r01 = cints[0];
		Integer r23 = cints[2];
		Integer r45 = cints[4];
		Integer r67 = cints[6];
		Integer r89 = cints[8];
		Integer r13 = cints[1];
		Integer r25 = cints[2];
		Integer r49 = cints[4];
		Integer r68 = cints[6];
		Integer r07 = cints[0];

		r01 *= cints[1];
		r23 *= cints[3];
		r45 *= cints[5];
		r67 *= cints[7];
		r89 *= cints[9];
		r13 *= cints[3];
		r25 *= cints[5];
		r49 *= cints[9];
		r68 *= cints[8];
		r07 *= cints[7];

		tests.runTest(r01.toString(), "-118458783711651021581285558191208126241981437713782681117257477640");
		tests.runTest(r23.toString(), "-11407853512132630592268596009512162507560");
		tests.runTest(r45.toString(), "-1519296563695929858");
		tests.runTest(r67.toString(), "-15274031514");
		tests.runTest(r89.toString(), "-3725");
		tests.runTest(r13.toString(), "11836286694244894161857011067984096726964182997900");
		tests.runTest(r25.toString(), "-21651875281354368391733664273582");
		tests.runTest(r49.toString(), "-96644833854");
		tests.runTest(r68.toString(), "1080350");
		tests.runTest(r07.toString(), "-4365556437451154097445405640564448339144");

		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	{		
		tests.reset("a/b");

		Integer r01 = cints[0] / cints[1];
		Integer r23 = cints[2] / cints[3];
		Integer r45 = cints[4] / cints[5];
		Integer r67 = cints[6] / cints[7];
		Integer r89 = cints[8] / cints[9];
		Integer r13 = cints[1] / cints[3];
		Integer r25 = cints[2] / cints[5];
		Integer r49 = cints[4] / cints[9];
		Integer r68 = cints[6] / cints[8];
		Integer r07 = cints[0] / cints[7];

		tests.runTest(r01.toString(), "-1287");
		tests.runTest(r23.toString(), "-7490");
		tests.runTest(r45.toString(), "0");
		tests.runTest(r67.toString(), "0");
		tests.runTest(r89.toString(), "0");
		tests.runTest(r13.toString(), "7771382367234");
		tests.runTest(r25.toString(), "-3946345969385");
		tests.runTest(r49.toString(), "-4353174");
		tests.runTest(r68.toString(), "1728");
		tests.runTest(r07.toString(), "-34944688835262458873323667247");

		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	{		
		tests.reset("a/=b");

		Integer r01 = cints[0];
		Integer r23 = cints[2];
		Integer r45 = cints[4];
		Integer r67 = cints[6];
		Integer r89 = cints[8];
		Integer r13 = cints[1];
		Integer r25 = cints[2];
		Integer r49 = cints[4];
		Integer r68 = cints[6];
		Integer r07 = cints[0];

		r01 /= cints[1];
		r23 /= cints[3];
		r45 /= cints[5];
		r67 /= cints[7];
		r89 /= cints[9];
		r13 /= cints[3];
		r25 /= cints[5];
		r49 /= cints[9];
		r68 /= cints[8];
		r07 /= cints[7];

		tests.runTest(r01.toString(), "-1287");
		tests.runTest(r23.toString(), "-7490");
		tests.runTest(r45.toString(), "0");
		tests.runTest(r67.toString(), "0");
		tests.runTest(r89.toString(), "0");
		tests.runTest(r13.toString(), "7771382367234");
		tests.runTest(r25.toString(), "-3946345969385");
		tests.runTest(r49.toString(), "-4353174");
		tests.runTest(r68.toString(), "1728");
		tests.runTest(r07.toString(), "-34944688835262458873323667247");

		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	{		
		tests.reset("a>>=b");

		Integer r20 = cints[2];
		Integer r21 = cints[2];
		Integer r22 = cints[2];
		Integer r23 = cints[2];
		Integer r24 = cints[2];
		Integer r50 = cints[5];
		Integer r51 = cints[5];
		Integer r52 = cints[5];
		Integer r53 = cints[5];
		Integer r54 = cints[5];

		r20 >>= 2;
		r21 >>= 14;
		r22 >>= 19;
		r23 >>= 24;
		r24 >>= 38;
		r50 >>= 3;
		r51 >>= 8;
		r52 >>= 13;
		r53 >>= 16;
		r54 >>= 20;

		tests.runTest(r20.toString(), "2310922309736578087058");
		tests.runTest(r21.toString(), "564190017025531759");
		tests.runTest(r22.toString(), "17630938032047867");
		tests.runTest(r23.toString(), "550966813501495");
		tests.runTest(r24.toString(), "33628345550");
		tests.runTest(r50.toString(), "-292792665");
		tests.runTest(r51.toString(), "-9149770");
		tests.runTest(r52.toString(), "-285930");
		tests.runTest(r53.toString(), "-35741");
		tests.runTest(r54.toString(), "-2233");

		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	{		
		tests.reset("a>>=b");

		Integer r20 = cints[2];
		Integer r21 = cints[2];
		Integer r22 = cints[2];
		Integer r23 = cints[2];
		Integer r24 = cints[2];
		Integer r50 = cints[5];
		Integer r51 = cints[5];
		Integer r52 = cints[5];
		Integer r53 = cints[5];
		Integer r54 = cints[5];

		r20 <<= 2;
		r21 <<= 14;
		r22 <<= 19;
		r23 <<= 27;
		r24 <<= 38;
		r50 <<= 3;
		r51 <<= 8;
		r52 <<= 13;
		r53 <<= 16;
		r54 <<= 20;

		tests.runTest(r20.toString(), "36974756955785249392936");
		tests.runTest(r21.toString(), "151448604490896381513465856");
		tests.runTest(r22.toString(), "4846355343708684208430907392");
		tests.runTest(r23.toString(), "1240666967989423157358312292352");
		tests.runTest(r24.toString(), "2540885950442338626269823574736896");
		tests.runTest(r50.toString(), "-18738730584");
		tests.runTest(r51.toString(), "-599639378688");
		tests.runTest(r52.toString(), "-19188460118016");
		tests.runTest(r53.toString(), "-153507680944128");
		tests.runTest(r54.toString(), "-2456122895106048");

		std::cout << tests.result() << std::endl;
		status &= tests.status();
	}

	/*
		Modulo:
		-1773273743103461260925877823936
		-1129409795571040706
		-1693718277
		-310237
		-124
		-1095703636533614875
		1008389444
		-29
		14
		-120504
	*/	
	
	return (int)(!status);
}

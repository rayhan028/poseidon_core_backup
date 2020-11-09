#include <iostream>
#include <numeric>
#include <boost/algorithm/string.hpp>

#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/DiagnosticPrinter.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/TargetSelect.h"

#include "config.h"
#include "graph_pool.hpp"

#include "p_context.hpp"
#include "JitFromScratch.hpp"
#include "qoperator.hpp"
#include "interpreter.hpp"

#include "qop.hpp"
#include "query.hpp"

#include <unistd.h>

#define SF_10
#define RUN_INDEXED
#define BUILD_INDEX

using namespace llvm;

graph_db_ptr graph1();
graph_db_ptr graph2();


algebra_optr create_is1_query();
algebra_optr create_is2_query();
algebra_optr create_is2_2_query();
algebra_optr create_is3_query();
algebra_optr create_is4_query();
algebra_optr create_is5_query();
algebra_optr create_is6_query();
algebra_optr create_is6_2_query();
algebra_optr create_is7_query();
algebra_optr create_is8_query();
algebra_optr create_iu1_query();
algebra_optr create_iu2_query();
algebra_optr create_iu3_query();
algebra_optr create_iu4_query();
algebra_optr create_iu5_query();
algebra_optr create_iu6_query();
algebra_optr create_iu8_query();

arg_builder create_args_q1() {
	properties_t props = {};
	arg_builder qargs;
	qargs.arg(1, "Person");
	qargs.arg(2, props);
	qargs.arg(3, ":isLocatedIn");
	qargs.arg(4, props);
	qargs.arg(5, ":hasInterest");
	qargs.arg(6, props);
	qargs.arg(7, ":studyAt");
	qargs.arg(8, props);
	qargs.arg(9, ":workAt");
	qargs.arg(10, props);
	qargs.arg(11, "Place");
	qargs.arg(12, "id");
	qargs.arg(13, 10);
	qargs.arg(14, "Tag");
	qargs.arg(15, "id");
	qargs.arg(16, 42);
	qargs.arg(17, "Organisation");
	qargs.arg(18, "id");
	qargs.arg(19, 21);
	qargs.arg(20, "Organisation");
	qargs.arg(21, "id");
	qargs.arg(22, 42);
	return qargs;
}

arg_builder create_args_q2() {
	properties_t props = {};
	arg_builder qargs;
	qargs.arg(1, "Person");
	qargs.arg(2, "id");
	qargs.arg(3, 22);
	qargs.arg(4, ":likes");
	qargs.arg(5, props);
	qargs.arg(6, "Post");
	qargs.arg(7, "id");
	qargs.arg(8, 42);
	return qargs;
}


arg_builder create_args_q3() {
	properties_t props = {};
	arg_builder qargs;
	qargs.arg(1, "Person");
	qargs.arg(2, "id");
	qargs.arg(3, 22);
	qargs.arg(4, ":likes");
	qargs.arg(5, props);
	qargs.arg(6, "Comment");
	qargs.arg(7, "id");
	qargs.arg(8, 42);
	return qargs;
}


arg_builder create_args_q4() {
	properties_t props = {};
	arg_builder qargs;
	qargs.arg(1, "Forum");
	qargs.arg(2, props);
	qargs.arg(3, ":hasModerator");
	qargs.arg(4, props);
	qargs.arg(5, ":hasTag");
	qargs.arg(6, props);
	qargs.arg(7, "Person");
	qargs.arg(8, "id");
	qargs.arg(9, 42);
	qargs.arg(10, "Tag");
	qargs.arg(11, "id");
	qargs.arg(12, 42);
	return qargs;
}

arg_builder create_args_q5() {
	properties_t props = {};
	arg_builder qargs;
	qargs.arg(1, "Forum");
	qargs.arg(2, "id");
	qargs.arg(3, 42);
	qargs.arg(4, ":hasMember");
	qargs.arg(5, props);
	qargs.arg(6, "Person");
	qargs.arg(7, "id");
	qargs.arg(8, 42);
	return qargs;
}

arg_builder create_args_q6() {
	properties_t props = {};
	arg_builder qargs;
	qargs.arg(1, "Post");
	qargs.arg(2, props);
	qargs.arg(3, ":hasCreator");
	qargs.arg(4, props);
	qargs.arg(5, ":containerOf");
	qargs.arg(6, props);
	qargs.arg(7, ":isLocatedIn");
	qargs.arg(8, props);
	qargs.arg(9, ":hasTag");
	qargs.arg(10, props);
	qargs.arg(11, "Person");
	qargs.arg(12, "id");
	qargs.arg(13, 42);
	qargs.arg(14, "Forum");
	qargs.arg(15, "id");
	qargs.arg(16, 42);
	qargs.arg(17, "Place");
	qargs.arg(18, "id");
	qargs.arg(19, 42);
	qargs.arg(20, "Tag");
	qargs.arg(21, "id");
	qargs.arg(22, 42);

	return qargs;
}

arg_builder create_args_q7() {
	properties_t props = {};
	arg_builder qargs;
	qargs.arg(1, "Comment");
	qargs.arg(2, props);
	qargs.arg(3, ":hasCreator");
	qargs.arg(4, props);
	qargs.arg(5, ":replyOf");
	qargs.arg(6, props);
	qargs.arg(7, ":isLocatedIn");
	qargs.arg(8, props);
	qargs.arg(9, ":hasTag");
	qargs.arg(10, props);
	qargs.arg(11, "Person");
	qargs.arg(12, "id");
	qargs.arg(13, 42);
	qargs.arg(14, "Post");
	qargs.arg(15, "id");
	qargs.arg(16, 42);
	qargs.arg(17, "Place");
	qargs.arg(18, "id");
	qargs.arg(19, 42);
	qargs.arg(20, "Tag");
	qargs.arg(21, "id");
	qargs.arg(22, 42);

	return qargs;
}


arg_builder create_args_q8() {
	properties_t props = {};
	arg_builder qargs;
	qargs.arg(1, "Person");
	qargs.arg(2, "id");
	qargs.arg(3, 42);
	qargs.arg(4, ":knows");
	qargs.arg(5, props);
	qargs.arg(6, "Person");
	qargs.arg(7, "id");
	qargs.arg(8, 42);
	return qargs;
}


double calc_avg(std::vector<double> &rt) {
	return std::accumulate(rt.begin(), rt.end(), 0.0) / (rt.size() * 1000);
}

using param_val = boost::variant<uint64_t, std::string, int>;
using params_tuple = std::vector<param_val>;

std::pair<double,double> run_ldbc_is1(JitFromScratch &jit, graph_db_ptr gdb, query_engine & qeng) {
	std::vector<params_tuple> parameters =
	{{(uint64_t)156, (uint64_t)7398, (uint64_t)4739, (uint64_t)1540, (uint64_t)30,
		 "Jose", "Rodriguez", "male", "1980-11-23", "2011-06-20T22:41:36.349+0000",
		 "170.25.1.157", "Chrome", "Acholi", "Jose@gmail.com",
		 2001, 2000},
	{(uint64_t)557, (uint64_t)13264, (uint64_t)6472, (uint64_t)930, (uint64_t)45062322265094,
		"Arjun", "Khan", "female", "1983-09-23", "2012-04-24T20:01:14.967+0000",
		"61.247.231.172", "Chrome", "Afrikaans", "Arjun@yahoo.com",
		2001, 2003},
	{(uint64_t)983, (uint64_t)13881, (uint64_t)7815, (uint64_t)1353, (uint64_t)85568671361729,
		"Tayfun", "Arikan", "male", "1981-09-20", "2011-07-13T14:20:09.883+0000",
		"82.150.64.102", "Internet Explorer", "Akan", "Tayfun@yahoo.com",
		2001, 2001},
	{(uint64_t)1257, (uint64_t)12995, (uint64_t)2772, (uint64_t)500, (uint64_t)50918298800281,
		"Marwan", "Abdullah", "female", "1989-02-25", "2011-02-08T11:40:36.445+0000",
		"188.247.64.114", "Chrome", "Albanian", "Marwan@education.edu",
		2001, 2009},
	{(uint64_t)1425, (uint64_t)11638, (uint64_t)4807, (uint64_t)953, (uint64_t)27,
		"Jie", "Chen", "male", "1986-05-12", "2012-04-04T16:47:09.234+0000",
		"1.1.5.2", "Firefox", "Amharic", "Jie@education.edu",
		2001, 2006},
	{(uint64_t)696, (uint64_t)494, (uint64_t)6655, (uint64_t)669, (uint64_t)60,
		"Manuel", "Ferreira", "male", "1987-07-17", "2011-11-11T15:20:57.997+0000",
		"91.230.40.128", "Internet Explorer", "Arabic", "Manuel@organisation.org",
		2001, 2007},
	{(uint64_t)451, (uint64_t)1460, (uint64_t)5385, (uint64_t)1359, (uint64_t)33,
		"Bertin", "Bona", "female", "1988-06-12", "2011-09-15T01:45:26.216+0000",
		"41.67.236.194", "Chrome", "Ashante", "Bertin@yahoo.com",
		2001, 2008},
	{(uint64_t)480, (uint64_t)3740, (uint64_t)5932, (uint64_t)650, (uint64_t)49525143970186,
		"Stefan", "Popescu", "female", "1987-08-03", "2011-03-23T14:26:08.586+0000",
		"46.214.42.87", "Internet Explorer", "Bajuni", "Stefan@hotmail.com",
		2001, 2007},
	{(uint64_t)858, (uint64_t)4238, (uint64_t)7708, (uint64_t)770, (uint64_t)71623222008184,
		"Kenji", "Yamamoto", "male", "1980-04-16", "2012-06-06T18:32:38.824+0000",
		"1.112.152.119", "Internet Explorer", "Basque", "Kenji@yahoo.com",
		2005, 2016},
	{(uint64_t)186, (uint64_t)12992, (uint64_t)3553, (uint64_t)1531, (uint64_t)89677380190134,
		"Sergio", "Mendez", "female", "1985-02-18", "2011-04-30T09:29:43.569+0000",
		"148.218.248.99", "Internet Explorer", "Behdini", "Sergio@education.edu",
		2001, 2000},
	{(uint64_t)560, (uint64_t)3832, (uint64_t)4642, (uint64_t)488, (uint64_t)48285385918120,
		"Álvaro", "Rodriguez", "female", "1985-08-09", "2011-03-18T16:55:55.015+0000",
		"186.84.146.57", "Firefox", "Danish", "Álvaro@organisation.org",
		2001, 2005},
	{(uint64_t)535, (uint64_t)1937, (uint64_t)6242, (uint64_t)1094, (uint64_t)41182469452391,
		"James", "Goenka", "male", "1987-11-18", "2011-10-17T06:35:29.397+0000",
		"203.215.62.54", "Chrome", "Dutch", "James@hotmail.com",
		2001, 2005},
	{(uint64_t)933, (uint64_t)12416, (uint64_t)2720, (uint64_t)585, (uint64_t)53,
		"Alberto", "Rodriguez", "female", "1989-09-29", "2012-08-11T00:35:43.152+0000",
		"148.233.180.14", "Firefox", "English", "Alberto@unversity.de",
		2001, 2007},
	{(uint64_t)1193, (uint64_t)7063, (uint64_t)5976, (uint64_t)369, (uint64_t)26,
		"Zeki", "Tekin", "female", "1986-05-12", "2011-10-28T20:20:10.606+0000",
		"31.210.116.188", "Firefox", "Finnish", "Zeki@education.edu",
		2001, 2009},
	{(uint64_t)750, (uint64_t)4942, (uint64_t)5931, (uint64_t)408, (uint64_t)98661986827197,
		"Faisal", "Ahmed", "male", "1984-05-10", "2010-09-10T19:37:05.752+0000",
		"116.0.49.100", "Firefox", "French", "Faisal@gmail.com",
		2001, 2006},
	{(uint64_t)697, (uint64_t)5748, (uint64_t)4932, (uint64_t)22, (uint64_t)42744053858505,
		"Roberto", "Garcia", "female", "1986-06-03", "2010-09-02T01:12:42.072+0000",
		"186.96.248.78", "Firefox", "Canadian", "Roberto@yahoo.com",
		2001, 2004},
	{(uint64_t)831, (uint64_t)9822, (uint64_t)2336, (uint64_t)1570, (uint64_t)63402364450002,
		"Chand", "Dissanayake", "female", "1983-12-28", "2010-06-25T18:55:00.647+0000",
		"202.124.188.187", "Firefox", "Georgian", "Chand@gmail.com",
		2001, 2006},
	{(uint64_t)1222, (uint64_t)12700, (uint64_t)2335, (uint64_t)1029, (uint64_t)59960056142195,
		"Daisuke", "Yamamoto", "female", "1989-10-01", "2010-02-03T02:19:45.623+0000",
		"27.84.209.47", "Internet Explorer", "German", "Daisuke@unversity.de",
		2001, 2003},
	{(uint64_t)483, (uint64_t)11741, (uint64_t)7056, (uint64_t)1201, (uint64_t)62,
		"Hans", "Johansson", "female", "1988-08-14", "2012-08-12T21:06:16.620+0000",
		"31.216.136.73", "Safari", "Gujarati", "Hans@education.edu",
		2001, 2009},
	{(uint64_t)158, (uint64_t)4346, (uint64_t)5193, (uint64_t)651, (uint64_t)45131216182357,
		"Jun", "Feng", "female", "1980-04-29", "2010-05-11T22:35:13.748+0000",
		"14.135.166.220", "Internet Explorer", "Hindi", "Jun@unversity.de",
		2001, 2008},
	{(uint64_t)135, (uint64_t)15212, (uint64_t)6474, (uint64_t)1414, (uint64_t)46913896635586,
		"Abhishek", "Nair", "male", "1988-05-24", "2011-05-18T04:51:49.317+0000",
		"27.123.216.37", "Chrome", "Hungarian", "Abhishek@organisation.org",
		2001, 2000},
	{(uint64_t)594, (uint64_t)6517, (uint64_t)4004, (uint64_t)370, (uint64_t)89062079816186,
		"Gyula", "Toth", "female", "1983-12-19", "2010-12-22T12:23:31.969+0000",
		"77.110.143.75", "Internet Explorer", "Italian", "Gyula@education.edu",
		2001, 2008},
	{(uint64_t)1357, (uint64_t)12689, (uint64_t)3562, (uint64_t)20, (uint64_t)35470767756645,
		"Ali", "Kamkar", "male", "1981-09-15", "2012-04-05T02:34:12.041+0000",
		"81.28.37.47", "Internet Explorer", "Jakartanese", "Ali@hotmail.com",
		2001, 2003},
	{(uint64_t)1253, (uint64_t)6929, (uint64_t)2722, (uint64_t)1550, (uint64_t)97354682054653,
		"Denis", "du Preez", "male", "1983-04-15", "2010-05-15T23:49:55.959+0000",
		"41.66.67.236", "Internet Explorer", "Japanese", "Denis@unversity.de",
		2001, 2001},
	{(uint64_t)1049, (uint64_t)5746, (uint64_t)6483, (uint64_t)1541, (uint64_t)81962633606441,
		"Paulo", "Silva", "female", "1984-06-03", "2012-01-06T14:48:00.760+0000",
		"192.207.200.153", "Chrome", "Kashmiri", "Paulo@hotmail.com",
		2001, 2003},
	{(uint64_t)714, (uint64_t)6901, (uint64_t)6658, (uint64_t)1548, (uint64_t)37853352464191,
		"Jose", "Garcia", "female", "1983-03-27", "2011-02-20T16:22:28.505+0000",
		"121.58.255.132", "Chrome", "Korean", "Jose@yahoo.com",
		2001, 2004},
	{(uint64_t)1191, (uint64_t)4953, (uint64_t)5600, (uint64_t)373, (uint64_t)45190810502156,
		"Zhi", "Zhang", "male", "1989-02-06", "2010-05-25T15:44:12.653+0000",
		"1.26.123.246", "Chrome", "Indonesian", "Zhi@education.edu",
		2001, 2003},
	{(uint64_t)472, (uint64_t)1955, (uint64_t)3120, (uint64_t)312, (uint64_t)58557575493548,
		"Stephane", "Charpentier", "female", "1982-03-01", "2012-05-11T23:22:43.942+0000",
		"62.100.156.105", "Chrome", "Kotokoli", "Stephane@education.edu",
		2001, 2009},
	{(uint64_t)937, (uint64_t)10928, (uint64_t)6082, (uint64_t)1352, (uint64_t)50347856215671,
		"Richard", "Johnson", "male", "1981-11-25", "2012-01-20T08:20:38.981+0000",
		"23.52.35.118", "Chrome", "Kurdish", "Richard@organisation.org",
		2001, 2002},
	{(uint64_t)1401, (uint64_t)15736, (uint64_t)6555, (uint64_t)1034, (uint64_t)91938870917592,
		"Mark", "Jones", "male", "1983-02-06", "2011-05-17T14:45:45.929+0000",
		"31.12.82.23", "Internet Explorer", "Macedonian", "Mark@education.edu",
		2001, 2001},
	{(uint64_t)1250, (uint64_t)1698, (uint64_t)3860, (uint64_t)501, (uint64_t)71597753674866,
		"Hao", "Li", "male", "1986-07-26", "2010-11-24T07:41:41.838+0000",
		"1.1.27.56", "Firefox", "Marathi", "Hao@education.edu",
		2001, 2003},
	{(uint64_t)1319, (uint64_t)3076, (uint64_t)6657, (uint64_t)908, (uint64_t)78847006912605,
		"Arjun", "Reddy", "female", "1987-11-20", "2010-01-25T05:32:01.685+0000",
		"49.128.110.148", "Chrome", "Marshallese", "Arjun@organisation.org",
		2001, 2006},
	{(uint64_t)759, (uint64_t)914, (uint64_t)7573, (uint64_t)1351, (uint64_t)85671553123089,
		"Carolina", "Alvarez", "male", "1989-03-27", "2010-01-30T14:29:41.352+0000",
		"161.196.190.232", "Firefox", "Mirpuri", "Carolina@hotmail.com",
		2001, 2007},
	{(uint64_t)136, (uint64_t)10910, (uint64_t)7829, (uint64_t)324, (uint64_t)79826776311898,
		"Elena", "Berman", "male", "1989-12-08", "2012-03-25T07:34:52.800+0000",
		"31.40.122.197", "Chrome", "Nepali", "Elena@yahoo.com",
		2001, 2009},
	{(uint64_t)931, (uint64_t)2838, (uint64_t)3820, (uint64_t)249, (uint64_t)85135125293346,
		"Artur", "Martins", "female", "1982-08-22", "2012-03-19T10:39:24.358+0000",
		"81.92.203.2", "Firefox", "Nigerian", "Artur@gmail.com",
		2001, 2009},
	{(uint64_t)1399, (uint64_t)14586, (uint64_t)5608, (uint64_t)688, (uint64_t)78563711446067,
		"Albert", "Mulder", "male", "1989-12-07", "2012-07-11T11:29:04.487+0000",
		"46.17.9.180", "Internet Explorer", "Norwegian", "Albert@gmail.com",
		2001, 2002},
	{(uint64_t)347, (uint64_t)14516, (uint64_t)4177, (uint64_t)671, (uint64_t)87447804747058,
		"Arun", "Khan", "female", "1980-05-11", "2011-09-16T02:33:00.249+0000",
		"61.95.154.15", "Firefox", "Portuguese", "Arun@hotmail.com",
		2001, 2009},
	{(uint64_t)494, (uint64_t)12917, (uint64_t)7574, (uint64_t)1136, (uint64_t)61547517575834,
		"R.", "Sharma", "female", "1989-03-07", "2010-02-21T08:30:00.280+0000",
		"27.96.87.58", "Firefox", "Romanian", "R.@yahoo.com",
		2001, 2000},
	{(uint64_t)1405, (uint64_t)2241, (uint64_t)3552, (uint64_t)652, (uint64_t)32,
		"Deepak", "Sharma", "male", "1985-05-21", "2012-03-22T09:19:45.542+0000",
		"61.16.143.255", "Chrome", "Russian", "Deepak@organisation.org",
		2001, 2009},
	{(uint64_t)1047, (uint64_t)7399, (uint64_t)4741, (uint64_t)478, (uint64_t)44643786537044,
		"Frank", "Brown", "male", "1980-04-27", "2012-06-27T00:20:24.000+0000",
		"49.128.1.245", "Firefox", "Serbian", "Frank@organisation.org",
		2001, 2005},
	{(uint64_t)1349, (uint64_t)2086, (uint64_t)1610, (uint64_t)425, (uint64_t)39493304717351,
		"Yang", "Li", "male", "1988-01-03", "2012-07-18T10:56:28.724+0000",
		"14.1.0.152", "Internet Explorer", "Shanghainese", "Yang@organisation.org",
		2001, 2000},
	{(uint64_t)471, (uint64_t)13401, (uint64_t)4809, (uint64_t)1030, (uint64_t)37118346077548,
		"John", "Rao", "male", "1980-07-06", "2010-06-07T10:32:44.025+0000",
		"27.123.217.139", "Firefox", "Swedish", "John@yahoo.com",
		2001, 2008},
	{(uint64_t)1312, (uint64_t)10575, (uint64_t)6080, (uint64_t)502, (uint64_t)86079724437548,
		"Wei", "He", "female", "1981-05-22", "2012-04-25T08:07:52.393+0000",
		"27.37.17.128", "Internet Explorer", "Tamil", "Wei@gmail.com",
		2001, 2000},
	{(uint64_t)184, (uint64_t)6755, (uint64_t)1876, (uint64_t)531, (uint64_t)54327429161426,
		"Wim", "Martens", "female", "1983-05-16", "2010-09-27T13:20:57.939+0000",
		"87.67.133.218", "Firefox", "Telugu", "Wim@education.edu",
		2001, 2001},
	{(uint64_t)830, (uint64_t)5747, (uint64_t)4640, (uint64_t)670, (uint64_t)13,
		"Juraj", "Bacsó", "female", "1980-02-04", "2011-07-29T05:45:20.857+0000",
		"188.121.165.198", "Safari", "Thai", "Juraj@education.edu",
		2001, 2003},
	{(uint64_t)474, (uint64_t)15062, (uint64_t)4370, (uint64_t)481, (uint64_t)75652492586804,
		"Mahmud Al", "Aboud", "female", "1985-09-20", "2010-11-29T12:16:06.632+0000",
		"41.208.98.75", "Firefox", "Tibetan", "Mahmud Al@gmail.com",
		2001, 2000},
	{(uint64_t)779, (uint64_t)537, (uint64_t)7524, (uint64_t)482, (uint64_t)11,
		"James", "Wilson", "male", "1986-11-11", "2012-07-19T16:12:09.563+0000",
		"24.31.187.242", "Chrome", "Turkish", "James@hotmail.com",
		2001, 2005},
	{(uint64_t)1299, (uint64_t)13692, (uint64_t)3946, (uint64_t)371, (uint64_t)95288680496395,
		"Alfredo", "Silva", "female", "1985-03-22", "2010-06-12T08:59:01.603+0000",
		"92.250.115.141", "Firefox", "Ukrainian", "Alfredo@unversity.de",
		2001, 2006},
	{(uint64_t)485, (uint64_t)9922, (uint64_t)2006, (uint64_t)326, (uint64_t)47147317419040,
		"Ken", "Kato", "male", "1986-05-02", "2010-12-18T06:36:20.469+0000",
		"1.1.84.223", "Firefox", "Urdu", "Ken@organisation.org",
		2001, 2005},
	{(uint64_t)1090, (uint64_t)8050, (uint64_t)6049, (uint64_t)1073, (uint64_t)54794073544820,
		"Choi", "Joo", "female", "1984-05-20", "2011-12-20T12:29:30.691+0000",
		"49.246.0.53", "Chrome", "Uzbek", "Choi@unversity.de",
		2001, 2006},
	{(uint64_t)918, (uint64_t)8887, (uint64_t)1608, (uint64_t)954, (uint64_t)40532629385749,
		"Ge", "Wang", "female", "1985-09-15", "2011-07-11T16:22:40.610+0000",
		"1.10.95.160", "Firefox", "Vietnamese", "Ge@organisation.org",
		2001, 2004},
	{(uint64_t)1311, (uint64_t)13327, (uint64_t)7525, (uint64_t)1507, (uint64_t)70402194557817,
		"Ammar", "Ben Dhifallah", "male", "1984-06-05", "2011-03-26T18:09:53.734+0000",
		"41.231.89.121", "Firefox", "Chinese", "Ammar@gmail.com",
		2001, 2005}};

	std::vector<double> runtimes(parameters.size());

	auto expr = create_is1_query();

	auto jit_start = std::chrono::system_clock::now();
	qeng.generate(jit, expr, false);
	auto jit_end = std::chrono::system_clock::now();
	auto comp = std::chrono::duration_cast<std::chrono::microseconds>(jit_end -
			jit_start).count();

	for(auto i = 0u; i < parameters.size(); i++) {
		auto params = parameters[i];
		properties_t new_node = {{"id", boost::any(boost::get<uint64_t>(params[4]))},
			{"firstName", boost::any(boost::get<std::string &>(params[5]))},
			{"lastName", boost::any(boost::get<std::string &>(params[6]))},
			{"gender", boost::any(boost::get<std::string &>(params[7]))},
			{"birthday", boost::any(boost::get<std::string &>(params[8]))},
			{"creationDate", boost::any(boost::get<std::string &>(params[9]))},
			{"locationIP", boost::any(boost::get<std::string &>(params[10]))},
			{"browserUsed", boost::any(boost::get<std::string &>(params[11]))},
			{"language", boost::any(boost::get<std::string &>(params[12]))},
			{"email", boost::any(boost::get<std::string &>(params[13]))}};

		properties_t study_at = {{"classYear", boost::any(boost::get<int>(params[14]))}};
		properties_t work_from = {{"workFrom", boost::any(boost::get<int>(params[15]))}};
		result_set rs;
		auto args = create_args_q1();
		args.arg(2, new_node);
		args.arg(8, study_at);
		args.arg(10, work_from);
		*args.args.data()[13] = boost::get<uint64_t>(params[0]);
		*args.args.data()[16] = boost::get<uint64_t>(params[1]);
		*args.args.data()[19] = boost::get<uint64_t>(params[2]);
		*args.args.data()[22] = boost::get<uint64_t>(params[3]);
		qeng.prepare(jit, gdb);
		auto start_qp = std::chrono::steady_clock::now();

		auto tx = gdb->begin_transaction();
		qeng.start_[0](gdb.get(), 0, gdb->get_nodes()->num_chunks(), tx, 1, &qeng.type_vec_[0], &rs, nullptr, qeng.finish_[0], 0, args.args.data());
		gdb->commit_transaction();

		auto end_qp = std::chrono::steady_clock::now();
		runtimes[i] = std::chrono::duration_cast<std::chrono::microseconds>(end_qp -
				start_qp).count();
		std::cout << rs << std::endl;
		std::cout << "Runtime: " << runtimes[i] << std::endl;
		joiner::rhs_input_.clear();
	}
	qeng.cleanup();
	return {comp, calc_avg(runtimes)};
}

std::pair<double,double> run_ldbc_is2(JitFromScratch &jit, graph_db_ptr gdb, query_engine & qeng) {
	std::vector<params_tuple> parameters =
	{{(uint64_t)7696610811483, (uint64_t)17592186084059, "2012-06-10T03:00:31.490+0000"},
		{(uint64_t)5497591771582, (uint64_t)28587302343570, "2011-09-13T07:32:44.590+0000"},
		{(uint64_t)8246356119147, (uint64_t)30786325580319, "2012-08-02T13:02:27.815+0000"},
		{(uint64_t)7696619441623, (uint64_t)24189255855276, "2012-06-19T01:12:51.762+0000"},
		{(uint64_t)5497579903673, (uint64_t)24189255869097, "2011-09-25T19:15:02.619+0000"},
		{(uint64_t)4398060000490, (uint64_t)6597069786049, "2011-07-06T09:44:09.620+0000"},
		{(uint64_t)8796130264302, (uint64_t)2199023283094, "2012-09-11T10:31:49.533+0000"},
		{(uint64_t)4947836933736, (uint64_t)6597069771464, "2011-07-15T10:33:05.681+0000"},
		{(uint64_t)7146847232530, (uint64_t)10995116325392, "2012-04-12T05:03:36.481+0000"},
		{(uint64_t)4947818025048, (uint64_t)15393162827013, "2011-07-27T15:41:28.272+0000"},
		{(uint64_t)2199029083789, (uint64_t)19791209361461, "2010-10-22T13:57:42.975+0000"},
		{(uint64_t)5497571046135, (uint64_t)19791209357939, "2011-09-25T05:08:20.362+0000"},
		{(uint64_t)4398055003782, (uint64_t)2199023282192, "2011-07-01T01:34:43.913+0000"},
		{(uint64_t)5497569177923, (uint64_t)10995116335147, "2011-09-16T14:45:40.874+0000"},
		{(uint64_t)5497583356004, (uint64_t)28587302386812, "2011-10-16T22:57:35.589+0000"},
		{(uint64_t)6047343741222, (uint64_t)8796093041362, "2012-01-06T03:43:38.714+0000"},
		{(uint64_t)7696586483135, (uint64_t)17592186103026, "2012-05-12T13:22:34.368+0000"},
		{(uint64_t)2199043507650, (uint64_t)32985348854773, "2010-10-18T04:19:39.180+0000"},
		{(uint64_t)3848309835304, (uint64_t)28587302378407, "2011-05-03T22:27:44.125+0000"},
		{(uint64_t)5497586586854, (uint64_t)15393162813786, "2011-10-13T06:47:29.235+0000"},
		{(uint64_t)7696587523025, (uint64_t)32985348862047, "2012-06-14T17:07:47.838+0000"},
		{(uint64_t)7146863384730, (uint64_t)17592186070889, "2012-03-17T07:19:47.498+0000"},
		{(uint64_t)4947813723017, (uint64_t)28587302371180, "2011-08-04T12:00:22.838+0000"},
		{(uint64_t)7146846091813, (uint64_t)19791209335491, "2012-03-16T12:31:14.554+0000"},
		{(uint64_t)7146855350632, (uint64_t)30786325584236, "2012-04-27T23:32:17.469+0000"},
		{(uint64_t)8246367660784, (uint64_t)8796093045679, "2012-08-03T20:05:20.701+0000"},
		{(uint64_t)549782071640, (uint64_t)24189255855187, "2010-04-29T11:37:48.380+0000"},
		{(uint64_t)7146848395723, (uint64_t)19791209323074, "2012-03-16T14:19:50.590+0000"},
		{(uint64_t)4947814529836, (uint64_t)32985348904794, "2011-08-31T17:16:22.711+0000"},
		{(uint64_t)7146827691957, (uint64_t)19791209302584, "2012-03-10T09:44:13.692+0000"},
		{(uint64_t)7146839849003, (uint64_t)17592186068239, "2012-05-03T15:49:15.651+0000"},
		{(uint64_t)8246358253712, (uint64_t)2199023301366, "2012-08-04T21:46:35.871+0000"},
		{(uint64_t)7696612540077, (uint64_t)6597069770050, "2012-05-19T08:26:44.685+0000"},
		{(uint64_t)8246353140578, (uint64_t)2199023288846, "2012-07-19T06:58:25.558+0000"},
		{(uint64_t)2199031372326, (uint64_t)8796093026236, "2010-09-12T07:42:49.274+0000"},
		{(uint64_t)2748799725071, (uint64_t)32985348855452, "2010-11-22T03:14:35.701+0000"},
		{(uint64_t)5497580846042, (uint64_t)13194139570394, "2011-09-26T15:08:43.468+0000"},
		{(uint64_t)3298554023503, (uint64_t)2199023323151, "2011-02-03T09:15:20.328+0000"},
		{(uint64_t)6047341783775, (uint64_t)28587302365986, "2011-12-05T02:19:53.068+0000"},
		{(uint64_t)2748819001611, (uint64_t)13194139598455, "2010-12-19T02:42:11.774+0000"},
		{(uint64_t)7696610395878, (uint64_t)21990232576546, "2012-05-20T05:32:06.269+0000"},
		{(uint64_t)719937, (uint64_t)32985348833794, "2010-02-25T02:23:45.469+0000"},
		{(uint64_t)4947805705816, (uint64_t)4398046580839, "2011-09-05T00:16:03.281+0000"},
		{(uint64_t)8796111909210, (uint64_t)2199023255940, "2012-09-11T07:36:00.703+0000"},
		{(uint64_t)7696612058226, (uint64_t)8796093025994, "2012-06-06T22:17:43.855+0000"},
		{(uint64_t)8246352063015, (uint64_t)13194139551277, "2012-08-01T05:54:26.067+0000"},
		{(uint64_t)6597095701089, (uint64_t)13194139574726, "2012-02-05T17:55:43.763+0000"},
		{(uint64_t)6047320927173, (uint64_t)19791209337321, "2012-01-03T14:26:45.356+0000"},
		{(uint64_t)3298552155690, (uint64_t)32985348893000, "2011-01-21T13:08:01.893+0000"},
		{(uint64_t)7146830484277, (uint64_t)26388279139007, "2012-05-06T20:14:13.703+0000"},
		{(uint64_t)4947839491145, (uint64_t)4398046524024, "2011-07-08T22:16:14.456+0000"},
		{(uint64_t)6047333264283, (uint64_t)26388279080215, "2012-01-04T23:51:06.509+0000"}};
	std::vector<double> runtimes(parameters.size());

	auto expr = create_is2_query();

	auto jit_start = std::chrono::system_clock::now();
	qeng.generate(jit, expr, false);
	auto jit_end = std::chrono::system_clock::now();
	auto comp = std::chrono::duration_cast<std::chrono::microseconds>(jit_end -
			jit_start).count();

	for(auto i = 0u; i < parameters.size(); i++) {
		auto params = parameters[i];
		result_set rs;

		properties_t rsh = {{"creationDate", boost::any(boost::get<std::string &>(params[2]))}};

		auto args = create_args_q2();
		args.arg(5, rsh);
		*args.args.data()[3] = boost::get<uint64_t>(params[1]);
		*args.args.data()[8] = boost::get<uint64_t>(params[0]);
		qeng.prepare(jit, gdb);
		auto start_qp = std::chrono::steady_clock::now();

		auto tx = gdb->begin_transaction();
		qeng.start_[0](gdb.get(), 0, gdb->get_nodes()->num_chunks(), tx, 1, &qeng.type_vec_[0], &rs, nullptr, qeng.finish_[0], 0, args.args.data());
		gdb->commit_transaction();

		auto end_qp = std::chrono::steady_clock::now();
		runtimes[i] = std::chrono::duration_cast<std::chrono::microseconds>(end_qp -
				start_qp).count();
		std::cout << rs << std::endl;
		std::cout << "Runtime: " << runtimes[i] << std::endl;
		joiner::rhs_input_.clear();
	}
	qeng.cleanup();
	return {comp, calc_avg(runtimes)};
}

std::pair<double,double> run_ldbc_is3(JitFromScratch &jit, graph_db_ptr gdb, query_engine & qeng) {
	std::vector<params_tuple> parameters = 
	{{(uint64_t)6597075130253, (uint64_t)2199023292039, "2012-02-09T08:35:10.880+0000"},
		{(uint64_t)6047335940961, (uint64_t)28587302394973, "2011-12-08T13:29:08.017+0000"},
		{(uint64_t)3848319520297, (uint64_t)4398046565852, "2011-04-08T19:46:44.321+0000"},
		{(uint64_t)7696621362002, (uint64_t)15393162816482, "2012-05-16T12:23:56.552+0000"},
		{(uint64_t)6047319597095, (uint64_t)26388279136569, "2011-12-13T21:12:45.740+0000"},
		{(uint64_t)6597075427118, (uint64_t)10995116291865, "2012-02-19T07:51:39.041+0000"},
		{(uint64_t)7696607323386, (uint64_t)2199023282192, "2012-06-13T05:41:00.248+0000"},
		{(uint64_t)1649285391175, (uint64_t)6597069777270, "2010-08-10T07:25:04.892+0000"},
		{(uint64_t)1099516667145, (uint64_t)21990232620104, "2010-05-23T14:25:32.722+0000"},
		{(uint64_t)5497573749960, (uint64_t)13194139552390, "2011-10-07T08:30:34.225+0000"},
		{(uint64_t)8246373177066, (uint64_t)28587302324409, "2012-08-05T06:55:48.820+0000"},
		{(uint64_t)6047321424823, (uint64_t)24189255869097, "2011-12-03T11:35:20.045+0000"},
		{(uint64_t)2199050343114, (uint64_t)10995116307781, "2010-09-15T15:08:36.845+0000"},
		{(uint64_t)6047325180051, (uint64_t)10995116329599, "2011-11-06T22:06:08.804+0000"},
		{(uint64_t)7696585512861, (uint64_t)21990232576546, "2012-06-09T03:13:29.236+0000"},
		{(uint64_t)6597095705405, (uint64_t)2199023280088, "2012-02-22T06:31:26.615+0000"},
		{(uint64_t)3298573289558, (uint64_t)21990232568995, "2011-01-07T19:20:16.628+0000"},
		{(uint64_t)3848296517945, (uint64_t)19791209339396, "2011-03-26T22:59:14.257+0000"},
		{(uint64_t)4398055404738, (uint64_t)26388279094964, "2011-06-15T21:05:17.107+0000"},
		{(uint64_t)3848320337477, (uint64_t)30786325590596, "2011-04-14T23:26:08.031+0000"},
		{(uint64_t)6047344355827, (uint64_t)28587302386812, "2011-12-10T19:47:09.694+0000"},
		{(uint64_t)7696612590700, (uint64_t)8796093026236, "2012-05-20T15:01:57.304+0000"},
		{(uint64_t)6597079151353, (uint64_t)10995116335147, "2012-02-22T14:56:02.654+0000"},
		{(uint64_t)7146864494880, (uint64_t)2199023270266, "2012-03-12T23:35:42.970+0000"},
		{(uint64_t)6047334117419, (uint64_t)19791209365758, "2012-01-03T00:19:58.703+0000"},
		{(uint64_t)7146828003400, (uint64_t)32985348867163, "2012-04-09T11:24:51.871+0000"},
		{(uint64_t)7146830870464, (uint64_t)2199023258582, "2012-04-28T17:00:44.742+0000"},
		{(uint64_t)5497570954495, (uint64_t)19791209337321, "2011-10-22T00:28:04.991+0000"},
		{(uint64_t)6597100579334, (uint64_t)10995116303733, "2012-02-26T16:25:52.095+0000"},
		{(uint64_t)8246341679136, (uint64_t)17592186096926, "2012-08-20T03:20:22.453+0000"},
		{(uint64_t)7146848891440, (uint64_t)2199023269986, "2012-04-14T16:50:43.593+0000"},
		{(uint64_t)6047328984642, (uint64_t)2199023300131, "2011-12-23T19:42:36.568+0000"},
		{(uint64_t)3848302244067, (uint64_t)10995116316433, "2011-04-16T23:20:14.277+0000"},
		{(uint64_t)3848294080815, (uint64_t)17592186103026, "2011-04-07T20:09:09.842+0000"},
		{(uint64_t)6597097365297, (uint64_t)24189255855276, "2012-01-14T01:39:24.230+0000"},
		{(uint64_t)7146862552587, (uint64_t)21990232571007, "2012-04-24T07:49:36.806+0000"},
		{(uint64_t)7696588236104, (uint64_t)32985348904794, "2012-05-13T09:09:11.943+0000"},
		{(uint64_t)6597098956931, (uint64_t)4398046536702, "2012-01-30T10:50:50.849+0000"},
		{(uint64_t)6047332794342, (uint64_t)32985348891281, "2011-12-19T17:57:33.202+0000"},
		{(uint64_t)549789466827, (uint64_t)28587302343570, "2010-04-07T14:54:01.801+0000"},
		{(uint64_t)3298553860410, (uint64_t)15393162854987, "2011-01-19T20:19:31.018+0000"},
		{(uint64_t)5497560899713, (uint64_t)13194139554675, "2011-10-12T12:31:55.288+0000"},
		{(uint64_t)6597098185498, (uint64_t)32985348898143, "2012-02-22T00:21:38.578+0000"},
		{(uint64_t)7696585487764, (uint64_t)15393162854260, "2012-05-09T03:46:42.874+0000"},
		{(uint64_t)2199034546270, (uint64_t)10995116348358, "2010-09-15T22:23:00.159+0000"},
		{(uint64_t)2199063192049, (uint64_t)8796093054594, "2010-09-19T19:21:56.037+0000"},
		{(uint64_t)3298555456954, (uint64_t)10995116325392, "2011-02-21T05:36:40.541+0000"},
		{(uint64_t)3848302338522, (uint64_t)6597069770050, "2011-03-23T08:42:48.145+0000"},
		{(uint64_t)7696596282784, (uint64_t)19791209361461, "2012-06-25T11:21:42.939+0000"},
		{(uint64_t)5497578630987, (uint64_t)17592186094267, "2011-10-16T13:22:39.508+0000"},
		{(uint64_t)8246350705571, (uint64_t)17592186084059, "2012-09-03T18:03:30.448+0000"},
		{(uint64_t)7696610248458, (uint64_t)2199023306228, "2012-05-09T14:30:52.776+0000"}};

	std::vector<double> runtimes(parameters.size());

	auto expr = create_is3_query();

	auto jit_start = std::chrono::system_clock::now();
	qeng.generate(jit, expr, false);
	auto jit_end = std::chrono::system_clock::now();
	auto comp = std::chrono::duration_cast<std::chrono::microseconds>(jit_end -
			jit_start).count();

	for(auto i = 0u; i < parameters.size(); i++) {
		auto params = parameters[i];
		result_set rs;

		properties_t rsh = {{"creationDate", boost::any(boost::get<std::string &>(params[2]))}};

		auto args = create_args_q3();
		args.arg(5, rsh);
		*args.args.data()[3] = boost::get<uint64_t>(params[1]);
		*args.args.data()[8] = boost::get<uint64_t>(params[0]);
		qeng.prepare(jit, gdb);
		auto start_qp = std::chrono::steady_clock::now();

		auto tx = gdb->begin_transaction();
		qeng.start_[0](gdb.get(), 0, gdb->get_nodes()->num_chunks(), tx, 1, &qeng.type_vec_[0], &rs, nullptr, qeng.finish_[0], 0, args.args.data());
		gdb->commit_transaction();

		auto end_qp = std::chrono::steady_clock::now();
		runtimes[i] = std::chrono::duration_cast<std::chrono::microseconds>(end_qp -
				start_qp).count();
		std::cout << rs << std::endl;
		std::cout << "Runtime: " << runtimes[i] << std::endl;
		joiner::rhs_input_.clear();
	}
	qeng.cleanup();
	return {comp, calc_avg(runtimes)};
}

std::pair<double,double> run_ldbc_is4(JitFromScratch &jit, graph_db_ptr gdb, query_engine & qeng) {
	std::vector<params_tuple> parameters =
	{{(uint64_t)4398046524024, (uint64_t)3076, (uint64_t)99279272422285,
		 "Album 9 of Deepak Chopra", "2012-03-23T13:12:15.991+0000"},
	{(uint64_t)10995116316433, (uint64_t)8037, (uint64_t)41423724166306,
		"Album 11 of Rahul Sharma", "2011-11-13T05:34:39.251+0000"},
	{(uint64_t)17592186096926, (uint64_t)10928, (uint64_t)81817525482877,
		"Album 5 of Hiroshi Sato", "2010-05-10T05:14:42.642+0000"},
	{(uint64_t)13194139597106, (uint64_t)3640, (uint64_t)83695001057287,
		"Album 19 of Koji Sato", "2012-02-19T09:46:02.175+0000"},
	{(uint64_t)8796093026236, (uint64_t)10905, (uint64_t)32173377921173,
		"Album 11 of A. Budjana", "2011-07-20T13:40:12.104+0000"},
	{(uint64_t)13194139538390, (uint64_t)9664, (uint64_t)77843225924249,
		"Album 7 of Jie Wei", "2011-07-29T10:31:12.046+0000"},
	{(uint64_t)17592186089083, (uint64_t)7399, (uint64_t)56235711290292,
		"Album 14 of Marcelo Souza", "2012-03-25T18:48:39.497+00000"},
	{(uint64_t)28587302324409, (uint64_t)11873, (uint64_t)27830761250403,
		"Album 12 of Chen Li", "2011-07-19T16:27:02.939+0000"},
	{(uint64_t)28587302365986, (uint64_t)8153, (uint64_t)78742869299612,
		"Album 3 of Chen Li", "2012-07-19T22:52:04.695+0000"},
	{(uint64_t)28587302394973, (uint64_t)9922, (uint64_t)85851768097604,
		"Album 0 of Jun Yang", "2011-04-05T07:56:33.014+0000"},
	{(uint64_t)26388279077767, (uint64_t)8158, (uint64_t)32641542835538,
		"Album 4 of Hamani Lambo", "2010-06-15T07:16:20.395+0000"},
	{(uint64_t)13194139554675, (uint64_t)3740, (uint64_t)8971594679854,
		"Album 24 of Tom Brown", "2011-12-06T04:48:01.301+0000"},
	{(uint64_t)19791209333857, (uint64_t)10985, (uint64_t)41915685907040,
		"Album 11 of Akira Kato", "2011-09-13T22:49:33.489+0000"},
	{(uint64_t)15393162813786, (uint64_t)11556, (uint64_t)23986622863447,
		"Album 19 of Jun Li", "2012-07-04T09:57:28.678+0000"},
	{(uint64_t)10995116320141, (uint64_t)11106, (uint64_t)86324813673483,
		"Album 2 of Ashok Bose", "2011-02-23T12:13:48.842+0000"},
	{(uint64_t)13194139570717, (uint64_t)2299, (uint64_t)78311943279472,
		"Album 8 of Hoang Yen Pham", "2012-01-26T21:18:54.016+0000"},
	{(uint64_t)2199023301366, (uint64_t)7062, (uint64_t)53648799777027,
		"Album 2 of Joseph Kumar", "2012-05-26T14:23:00.940+0000"},
	{(uint64_t)10995116348358, (uint64_t)2260, (uint64_t)93837538769399,
		"Album 16 of Takashi Ito", "2010-05-14T07:18:26.691+0000"},
	{(uint64_t)8796093074182, (uint64_t)12590, (uint64_t)85771080543032,
		"Album 1 of Francisco Garcia", "2011-12-30T10:15:02.748+0000"},
	{(uint64_t)2199023288846, (uint64_t)4423, (uint64_t)23090287017052,
		"Album 16 of Jie Chen", "2012-02-13T15:47:54.210+0000"},
	{(uint64_t)17592186070889, (uint64_t)3832, (uint64_t)52198736025161,
		"Album 24 of Mahmoud Akef", "2010-10-04T14:15:38.355+0000"},
	{(uint64_t)32985348891281, (uint64_t)6230, (uint64_t)75043363163784,
		"Album 14 of Dimitrios Dionysiou", "2012-08-16T23:13:16.798+0000"},
	{(uint64_t)40261, (uint64_t)10646, (uint64_t)61974299718501,
		"Album 15 of Ilhan Koksal", "2010-06-07T08:06:51.284+0000"},
	{(uint64_t)32985348898143, (uint64_t)5090, (uint64_t)94870727309166,
		"Album 16 of Wei Li", "2011-06-29T13:41:54.892+0000"},
	{(uint64_t)2199023323151, (uint64_t)12994, (uint64_t)78731175196931,
		"Album 6 of Antonio Mendez", "2012-03-27T06:21:55.988+0000"},
	{(uint64_t)19791209365758, (uint64_t)4539, (uint64_t)75672547602093,
		"Album 11 of Anastasia Anwar", "2012-06-25T05:12:43.588+0000"},
	{(uint64_t)2199023280088, (uint64_t)14936, (uint64_t)31617488734784,
		"Wall of Francis Garcia", "2010-12-28T16:22:56.638+0000"},
	{(uint64_t)26388279139007, (uint64_t)4238, (uint64_t)90624906806921,
		"Album 14 of Asha-Rose Mtengeti Bomani", "2011-09-18T10:46:17.421+0000"},
	{(uint64_t)8796093045679, (uint64_t)6849, (uint64_t)12310302991365,
		"Album 9 of Takeshi Yamada", "2012-03-23T07:47:26.269+0000"},
	{(uint64_t)4398046565737, (uint64_t)15296, (uint64_t)24790356571323,
		"Wall of Prakash Kapoor", "2010-07-24T02:29:18.166+0000"},
	{(uint64_t)19791209302584, (uint64_t)12992, (uint64_t)15426509028164,
		"Album 19 of Antonio Donati", "2011-05-11T17:32:41.614+0000"},
	{(uint64_t)21990232568995, (uint64_t)1917, (uint64_t)39544187297339,
		"Album 12 of Hossein Forouhar", "2011-02-16T18:04:55.572+0000"},
	{(uint64_t)6597069774872, (uint64_t)917, (uint64_t)76235522247549,
		"Album 30 of Karim Charbib", "2012-03-23T13:49:29.565+0000"},
	{(uint64_t)13194139605772, (uint64_t)4346, (uint64_t)92563475408764,
		"Album 5 of Ammar David", "2012-04-26T01:46:55.675+0000"},
	{(uint64_t)30786325626045, (uint64_t)15934, (uint64_t)84230313240664,
		"Album 14 of Jonas Larsson", "2011-10-02T07:16:37.214+0000"},
	{(uint64_t)32985348891170, (uint64_t)12689, (uint64_t)30212182714276,
		"Album 27 of Luciano Guelleh", "2011-01-28T04:03:22.834+0000"},
	{(uint64_t)21990232600284, (uint64_t)15062, (uint64_t)71004457789964,
		"Album 4 of Caroline Oliveira", "2011-03-02T21:48:01.948+0000"},
	{(uint64_t)13194139584639, (uint64_t)12937, (uint64_t)29888385856871,
		"Album 7 of Paul Niculiţă", "2012-06-08T16:48:40.069+0000"},
	{(uint64_t)15393162854987, (uint64_t)12405, (uint64_t)24254949427409,
		"Album 24 of Cheikh Diop", "2010-02-06T08:07:13.498+0000"},
	{(uint64_t)4398046565852, (uint64_t)15360, (uint64_t)88462473115077,
		"Album 7 of Ai Kobayashi", "2012-05-28T07:07:52.515+0000"},
	{(uint64_t)4398046553309, (uint64_t)2218, (uint64_t)45591548705195,
		"Album 20 of A. Singh", "2010-08-11T06:44:32.488+0000"},
	{(uint64_t)17592186096316, (uint64_t)13365, (uint64_t)51990098908145,
		"Album 20 of Jharana Gurung", "2010-03-08T00:22:32.844+0000"},
	{(uint64_t)24189255869097, (uint64_t)13212, (uint64_t)46517908971654,
		"Album 7 of Georges Mertens", "2011-01-11T01:50:36.447+0000"},
	{(uint64_t)17592186076766, (uint64_t)874, (uint64_t)82894407990262,
		"Album 19 of A. Kumar", "2012-05-14T08:40:51.985+0000"},
	{(uint64_t)13194139562958, (uint64_t)6272, (uint64_t)74251410764995,
		"Album 3 of Bing Li", "2010-12-21T20:34:30.219+0000"},
	{(uint64_t)28587302324269, (uint64_t)12700, (uint64_t)64137449001235,
		"Album 16 of Alberto Rojas", "2012-05-10T14:52:20.870+0000"},
	{(uint64_t)2199023315021, (uint64_t)5747, (uint64_t)12652953983353,
		"Album 1 of Yang Wang", "2011-06-02T06:05:49.496+0000"},
	{(uint64_t)18586, (uint64_t)15003, (uint64_t)87008747993745,
		"Album 1 of Mirza Kalich Khan", "2012-01-17T11:03:18.889+0000"},
	{(uint64_t)26388279080215, (uint64_t)3184, (uint64_t)18540253760217,
		"Album 4 of Koji Suzuki", "2011-01-12T18:33:56.454+0000"},
	{(uint64_t)17592186103026, (uint64_t)537, (uint64_t)9883535318720,
		"Album 18 of Alexei Basov", "2011-08-12T06:15:11.116+0000"},
	{(uint64_t)13194139562905, (uint64_t)7449, (uint64_t)13633885200986,
		"Album 11 of Albaye Papa Faye", "2010-04-12T00:42:25.312+0000"},
	{(uint64_t)32985348854773, (uint64_t)11628, (uint64_t)62648859675677,
		"Album 28 of Ivan Berman", "2010-03-05T09:35:04.464+0000"}};	

	std::vector<double> runtimes(parameters.size());

	auto expr = create_is4_query();

	auto jit_start = std::chrono::system_clock::now();
	qeng.generate(jit, expr, false);
	auto jit_end = std::chrono::system_clock::now();
	auto comp = std::chrono::duration_cast<std::chrono::microseconds>(jit_end -
			jit_start).count();

	for(auto i = 0u; i < parameters.size(); i++) {
		auto params = parameters[i];
		result_set rs;

		properties_t n = {{"id", boost::any(boost::get<uint64_t>(params[2]))},
			{"title", boost::any(boost::get<std::string &>(params[3]))},
			{"creationDate", boost::any(boost::get<std::string &>(params[4]))}};
		auto args = create_args_q4();
		args.arg(2, n);
		*args.args.data()[9] = boost::get<uint64_t>(params[0]);
		*args.args.data()[12] = boost::get<uint64_t>(params[1]);
		qeng.prepare(jit, gdb);
		auto start_qp = std::chrono::steady_clock::now();

		auto tx = gdb->begin_transaction();
		qeng.start_[0](gdb.get(), 0, gdb->get_nodes()->num_chunks(), tx, 1, &qeng.type_vec_[0], &rs, nullptr, qeng.finish_[0], 0, args.args.data());
		gdb->commit_transaction();

		auto end_qp = std::chrono::steady_clock::now();
		runtimes[i] = std::chrono::duration_cast<std::chrono::microseconds>(end_qp -
				start_qp).count();
		std::cout << rs << std::endl;
		std::cout << "Runtime: " << runtimes[i] << std::endl;
		joiner::rhs_input_.clear();
	}
	qeng.cleanup();
	return {comp, calc_avg(runtimes)};

}

std::pair<double,double> run_ldbc_is5(JitFromScratch &jit, graph_db_ptr gdb, query_engine & qeng) {
	std::vector<params_tuple> parameters =
	{{(uint64_t)13194139551277, (uint64_t)6047318168225, "2012-01-06T11:21:05.645+0000"},
		{(uint64_t)4398046519418, (uint64_t)6047313990715, "2011-12-06T10:34:38.084+0000"},
		{(uint64_t)21990232568995, (uint64_t)6597074002362, "2012-02-03T22:51:24.341+0000"},
		{(uint64_t)8796093025994, (uint64_t)1099513771058, "2010-06-20T01:13:33.122+0000"},
		{(uint64_t)28587302324269, (uint64_t)4947806600502, "2011-08-24T01:58:19.243+0000"},
		{(uint64_t)15393162854260, (uint64_t)6597072951099, "2012-02-03T04:09:16.164+0000"},
		{(uint64_t)15393162816482, (uint64_t)4947807594549, "2011-08-19T14:18:38.876+0000"},
		{(uint64_t)24189255835727, (uint64_t)549761095873, "2010-03-24T16:24:00.873+0000"},
		{(uint64_t)18586, (uint64_t)3848291846808, "2011-04-29T09:55:42.932+0000"},
		{(uint64_t)2199023306228, (uint64_t)7696585662070, "2012-06-28T23:28:43.400+0000"},
		{(uint64_t)13194139562905, (uint64_t)1099518968854, "2010-05-06T14:17:54.905+0000"},
		{(uint64_t)26388279107480, (uint64_t)4947805491082, "2011-08-26T15:57:55.442+0000"},
		{(uint64_t)2199023292039, (uint64_t)4398046542220, "2011-05-11T19:48:15.834+0000"},
		{(uint64_t)4398046568165, (uint64_t)4947803384199, "2011-07-12T13:23:26.830+0000"},
		{(uint64_t)32985348855452, (uint64_t)4947805520958, "2011-07-31T16:46:46.762+0000"},
		{(uint64_t)6597069782503, (uint64_t)6047318231683, "2011-12-13T07:14:37.651+0000"},
		{(uint64_t)15393162811721, (uint64_t)7696586688994, "2012-07-04T06:58:38.510+0000"},
		{(uint64_t)30786325577812, (uint64_t)5497560315167, "2011-09-27T05:46:26.874+0000"},
		{(uint64_t)2199023280088, (uint64_t)1649269591835, "2010-08-07T18:26:11.058+0000"},
		{(uint64_t)10995116309820, (uint64_t)2748783317586, "2010-12-18T14:45:47.692+0000"},
		{(uint64_t)15393162813786, (uint64_t)5497561297447, "2011-09-21T04:18:51.798+0000"},
		{(uint64_t)10995116335147, (uint64_t)4398051794799, "2011-05-31T02:36:47.724+0000"},
		{(uint64_t)13194139598455, (uint64_t)2748780192293, "2010-12-24T08:54:53.514+0000"},
		{(uint64_t)4398046553309, (uint64_t)7146831920774, "2012-03-25T15:15:56.331+0000"},
		{(uint64_t)71623, (uint64_t)6597072972025, "2012-02-14T09:33:26.502+0000"},
		{(uint64_t)26388279136569, (uint64_t)4398047613537, "2011-05-18T17:15:55.137+0000"},
		{(uint64_t)21990232627526, (uint64_t)6597077121477, "2012-02-26T16:50:58.201+0000"},
		{(uint64_t)32985348858567, (uint64_t)4947803403716, "2011-08-23T08:01:10.116+0000"},
		{(uint64_t)6597069800716, (uint64_t)6597073976542, "2012-01-14T06:44:02.238+0000"},
		{(uint64_t)32985348893000, (uint64_t)3848293924699, "2011-04-10T01:04:57.392+0000"},
		{(uint64_t)21990232576546, (uint64_t)7696585631874, "2012-07-05T07:51:01.033+0000"},
		{(uint64_t)13194139554675, (uint64_t)3848293873022, "2011-04-28T12:26:00.550+0000"},
		{(uint64_t)10995116303733, (uint64_t)6597070841811, "2012-02-05T02:59:59.642+0000"},
		{(uint64_t)32985348891012, (uint64_t)4947803423060, "2011-07-26T01:57:42.364+0000"},
		{(uint64_t)21990232584495, (uint64_t)7696587743889, "2012-06-20T00:21:18.801+0000"},
		{(uint64_t)17592186094267, (uint64_t)4398048627310, "2011-06-21T11:08:13.271+0000"},
		{(uint64_t)29060, (uint64_t)6047315029594, "2012-01-04T13:35:08.148+0000"},
		{(uint64_t)26388279115622, (uint64_t)4947808680650, "2011-07-26T17:42:37.689+0000"},
		{(uint64_t)2199023315021, (uint64_t)8246337274112, "2012-08-26T06:13:40.505+0000"},
		{(uint64_t)21990232593563, (uint64_t)7696582527997, "2012-06-06T09:26:09.042+0000"},
		{(uint64_t)4398046580839, (uint64_t)7696584568962, "2012-05-31T10:21:33.446+0000"},
		{(uint64_t)2199023255940, (uint64_t)8246338331876, "2012-08-24T09:33:03.494+0000"},
		{(uint64_t)30786325584236, (uint64_t)6597072916290, "2012-01-10T07:57:00.141+0000"},
		{(uint64_t)13194139568647, (uint64_t)6047321295159, "2011-11-18T11:55:49.417+0000"},
		{(uint64_t)30786325642083, (uint64_t)4398047565098, "2011-05-21T21:28:33.750+0000"},
		{(uint64_t)21990232571007, (uint64_t)4398050749920, "2011-05-15T12:54:50.959+0000"},
		{(uint64_t)4398046515646, (uint64_t)7146826646483, "2012-04-30T19:01:22.025+0000"},
		{(uint64_t)17592186103026, (uint64_t)4398050789584, "2011-06-04T12:35:32.716+0000"},
		{(uint64_t)13194139570717, (uint64_t)1099513728647, "2010-06-20T10:03:00.169+0000"},
		{(uint64_t)8796093054594, (uint64_t)6597069831800, "2012-01-23T09:29:31.870+0000"},
		{(uint64_t)30786325626045, (uint64_t)6597076075383, "2012-02-13T15:14:43.067+0000"},
		{(uint64_t)4398046565852, (uint64_t)6597075101214, "2012-02-13T02:44:41.505+0000"}};
	std::vector<double> runtimes(parameters.size());

	auto expr = create_is5_query();

	auto jit_start = std::chrono::system_clock::now();
	qeng.generate(jit, expr, false);
	auto jit_end = std::chrono::system_clock::now();
	auto comp = std::chrono::duration_cast<std::chrono::microseconds>(jit_end -
			jit_start).count();

	for(auto i = 0u; i < parameters.size(); i++) {
		auto params = parameters[i];
		result_set rs;

		properties_t n = {{"creationDate", boost::any(boost::get<std::string &>(params[2]))}};
		auto args = create_args_q5();
		args.arg(5, n);
		*args.args.data()[3] = boost::get<uint64_t>(params[1]);
		*args.args.data()[8] = boost::get<uint64_t>(params[0]);
		qeng.prepare(jit, gdb);
		auto start_qp = std::chrono::steady_clock::now();

		auto tx = gdb->begin_transaction();
		qeng.start_[0](gdb.get(), 0, gdb->get_nodes()->num_chunks(), tx, 1, &qeng.type_vec_[0], &rs, nullptr, qeng.finish_[0], 0, args.args.data());
		gdb->commit_transaction();

		auto end_qp = std::chrono::steady_clock::now();
		runtimes[i] = std::chrono::duration_cast<std::chrono::microseconds>(end_qp -
				start_qp).count();
		std::cout << rs << std::endl;
		std::cout << "Runtime: " << runtimes[i] << std::endl;
		joiner::rhs_input_.clear();
	}
	qeng.cleanup();
	return {comp, calc_avg(runtimes)};


}

std::pair<double,double> run_ldbc_is6(JitFromScratch &jit, graph_db_ptr gdb, query_engine & qeng) {
	std::vector<params_tuple> parameters =
	{{(uint64_t)26388279139007, (uint64_t)7146830824739, (uint64_t)34,
		 (uint64_t)13088, (uint64_t)7146847411148,
		 "", "2012-04-06T10:09:52.185+0000", "27.54.182.164", "Firefox", "Acholi", "About John Howard, "
			 "He was a close friend and loyal supporteAbout Fidel Castro, 1953. Imprisoned for a year, he "
			 "traveledAbout Mahmoud Ahmadinejad,  and Israel refers to the relations betwAbout Cicer",
		 197},
	{(uint64_t)4398046550229, (uint64_t)1099512767619, (uint64_t)30,
		(uint64_t)11628, (uint64_t)6047346033775,
		"", "2011-12-24T06:56:19.013+0000", "27.98.216.118", "Internet Explorer", "Afrikaans", "About "
			"Freddie Mercury, ngwriter, Mercury composed many hits for Queen, including Bohemian Rha",
		93},
	{(uint64_t)19791209337321, (uint64_t)2199024366028, (uint64_t)54,
		(uint64_t)8153, (uint64_t)8246344354698,
		"", "2012-09-05T20:14:00.995+0000", "82.114.190.233", "Firefox", "Akan", "About Georg Wilhelm "
			"Friedrich Hegel, g positions, including both his admirers and his detractors. His influential concept",
		121},
	{(uint64_t)28587302347895, (uint64_t)7696584614703, (uint64_t)6,
		(uint64_t)9822, (uint64_t)6597089548033,
		"", "2012-01-30T07:20:07.584+0000", "27.131.212.97", "Firefox", "Albanian", "About Alexander "
			"Downer, gest-serving in AAbout Woody Guthrie, ied from complicaAbout Molière, ing, co",
		101},
	{(uint64_t)13194139570394, (uint64_t)1649269558423, (uint64_t)43,
		(uint64_t)6822, (uint64_t)6047339393140,
		"", "2011-12-24T22:25:47.475+0000", "27.112.125.5", "Chrome", "Amharic", "About Ralph Bakshi, "
			"stablished an alternative to mainstream animation through independent and adul",
		98},
	{(uint64_t)10995116344920, (uint64_t)7146826680797, (uint64_t)35,
		(uint64_t)10641, (uint64_t)3848294113709,
		"", "2011-05-04T02:03:49.468+0000", "116.71.83.189", "Firefox", "Arabic", "About Wolfgang "
			"Amadeus Mozart,  Haydn wrote that posterity wAbout Henry II of England, changes Henry introduced d",
		113},
	{(uint64_t)8796093045679, (uint64_t)7146829783942, (uint64_t)110,
		(uint64_t)12316, (uint64_t)6597072249049,
		"", "2012-01-10T10:30:54.219+0000", "1.203.22.168", "Firefox", "Ashante", "About William the "
			"Conqueror, lative Edward the Confessor. There were other potential About Pope Pius X, neris ",
		110},
	{(uint64_t)28587302350407, (uint64_t)7696583507802, (uint64_t)14,
		(uint64_t)4870, (uint64_t)2199024019128,
		"", "2010-10-12T07:45:49.772+0000", "202.83.174.123", "Chrome", "Bajuni", "About Shapur II, "
			"sometimes given in EnAbout Mark Twain, anghorne Clemens (NovAbout Donna Summer, ive-time Grammy AwardAbo",
		121},
	{(uint64_t)6597069807409, (uint64_t)2199026467075, (uint64_t)102,
		(uint64_t)15062, (uint64_t)7696596799603,
		"", "2012-06-18T22:13:42.761+0000", "88.197.53.187", "Chrome", "Basque", "About Joseph Haydn, "
			"terházy family on their remote About Johann Sebastian Bach, rector of m",
		91},
	{(uint64_t)28587302394973, (uint64_t)1099511666036, (uint64_t)37,
		(uint64_t)3567, (uint64_t)4398077230263,
		"", "2011-06-09T17:46:52.876+0000", "110.5.111.46", "Chrome", "Behdini", "About Al Jolson, "
			"s. After a period of inactivity, his stardom returned with the 1946 Oscar-winning biographical "
			"film, The Jolson Story. Larr",
		140},
	{(uint64_t)15393162797174, (uint64_t)4398047572911, (uint64_t)13,
		(uint64_t)5747, (uint64_t)3298554923139,
		"", "2011-01-25T11:41:28.275+0000", "14.102.237.94", "Firefox", "Danish", "About Hamid Karzai, "
			"political figures to serve a six month term as Chairman of the Interim A",
		92},
	{(uint64_t)26388279107480, (uint64_t)1649270681785, (uint64_t)76,
		(uint64_t)15934, (uint64_t)7696589610704,
		"", "2012-05-20T00:13:04.110+0000", "46.44.233.39", "Chrome", "Dutch", "About Vinicius de "
			"Moraes,  important albuAbout Algeria, west by WesternAbout Vol. 3... Life and Times of S. Carter, ",
		116},
	{(uint64_t)32985348862047, (uint64_t)7696586646185, (uint64_t)9,
		(uint64_t)6012, (uint64_t)5497567727600,
		"", "2011-10-06T13:57:09.735+0000", "103.1.102.59", "Chrome", "English", "About Wolfgang Amadeus "
			"Mozart, uch mythologized. He was survivAbout Sammy Sosa, istory to ",
		90},
	{(uint64_t)19791209357939, (uint64_t)4947804499986, (uint64_t)53,
		(uint64_t)14391, (uint64_t)2199043928328,
		"", "2010-10-12T08:24:49.772+0000", "41.75.246.196", "Chrome", "Finnish", "About Wolfgang Amadeus "
			"Mozart, posed from the age of About Shapur II, sian Sassanid Em",
		86},
	{(uint64_t)10995116316433, (uint64_t)549755873520, (uint64_t)74,
		(uint64_t)15399, (uint64_t)6597090309775,
		"", "2012-02-10T18:47:53.501+0000", "195.39.219.224", "Chrome", "French", "About Diego "
			"Maradona, all time. He won FIFA Player of the Century award which was to be de",
		90},
	{(uint64_t)13194139554675, (uint64_t)7146828771259, (uint64_t)105,
		(uint64_t)11566, (uint64_t)5497575256515,
		"", "2011-10-11T14:45:52.087+0000", "1.4.4.180", "Firefox", "Canadian", "About Confucius,  "
			"developed intoAbout Raphael,  February 27, 1About Mariano Rivera,  le",
		87},
	{(uint64_t)13194139564716, (uint64_t)6047316070706, (uint64_t)62,
		(uint64_t)10985, (uint64_t)3298563285763,
		"", "2011-01-15T11:41:48.825+0000", "109.233.196.119", "Firefox", "Georgian", "About "
			"Slavoj Žižek,  University of Ljubljana, Slovenia, and aAbout Tina Turner, er musical "
			"career led to film roles, beg",
		120},
	{(uint64_t)10995116329599, (uint64_t)4398049751338, (uint64_t)90,
		(uint64_t)5541, (uint64_t)5497581256744,
		"", "2011-09-08T09:41:52.459+0000", "197.15.75.92", "Firefox", "German", "About Giacomo "
			"Puccini, Whilst his work is essentially based on traditional late-19th century Italian",
		100},
	{(uint64_t)21990232568995, (uint64_t)4398047613447, (uint64_t)71,
		(uint64_t)11638, (uint64_t)2199051902931,
		"", "2010-10-07T11:50:34.984+0000", "1.117.246.121", "Firefox", "Gujarati", "About "
			"Augustine of Hippo, tine developed his own approach to philosophy and theology, accommodatin",
		98},
	{(uint64_t)4398046565852, (uint64_t)2748785369563, (uint64_t)60,
		(uint64_t)3640, (uint64_t)8246338760598,
		"", "2012-08-11T01:41:02.693+0000", "27.116.47.59", "Chrome", "Hindi", "About "
			"Augustine of Hippo, lic Church as a spiritual City of God, distinct from the material Eart",
		96},
	{(uint64_t)13194139603906, (uint64_t)5497564438227, (uint64_t)98,
		(uint64_t)7619, (uint64_t)3848301820044,
		"", "2011-04-16T04:34:56.918+0000", "46.34.186.223", "Chrome", "Hungarian",
		"About Charles V, Holy Roman Emperor, he Peace of Augsburg. Charles alAbout "
			"Charles Darwin, Pa",
		93},
	{(uint64_t)26388279115622, (uint64_t)3848293931327, (uint64_t)3,
		(uint64_t)12405, (uint64_t)5497563321542,
		"", "2011-09-14T15:02:40.327+0000", "24.56.74.221", "Chrome", "Italian", "About "
			"Hector Berlioz, ioz made significanAbout Margaret Thatcher, nservative policiesAbout J",
		92},
	{(uint64_t)4398046565737, (uint64_t)5497561337803, (uint64_t)104,
		(uint64_t)12152, (uint64_t)8246339998202,
		"", "2012-08-28T06:27:35.569+0000", "31.222.79.241", "Chrome", "Jakartanese", "About Wolfgang "
			"Amadeus Mozart, ar of classical composers. Mozart showed prodigious ability from his earliest",
		108},
	{(uint64_t)17592186094267, (uint64_t)8246337304297, (uint64_t)67,
		(uint64_t)3076, (uint64_t)2199052283978,
		"", "2010-09-14T15:16:12.294+0000", "12.76.116.147", "Chrome", "Japanese", "About Sonia "
			"Gandhi, ed as the leader oAbout Mel Tormé, s also a jazz compAbout Come On Eileen, o. The so",
		104},
	{(uint64_t)10995116320141, (uint64_t)8796097293253, (uint64_t)28,
		(uint64_t)6880, (uint64_t)4947804124283,
		"", "2011-09-04T12:53:28.779+0000", "61.246.43.188", "Chrome", "Kashmiri", "About Rumi, sance "
			"(in the 8th/9th century) started in regions of Sistan, Khorāsān and Transoxiana and",
		101},
	{(uint64_t)19791209363124, (uint64_t)4398050766816, (uint64_t)52,
		(uint64_t)6728, (uint64_t)1099518988936,
		"", "2010-06-16T00:34:21.099+0000", "27.103.81.73", "Internet Explorer", "Korean", "About Siad "
			"Barre, rs of military rule, Barre's About Nero,  favorable light. Some sourceAbout Bruce "
			"Springsteen, ecordings have included both About Janet Jackson, rd prod",
		170},
	{(uint64_t)18586, (uint64_t)5497565491148, (uint64_t)101,
		(uint64_t)13340, (uint64_t)4947810502338,
		"", "2011-07-29T14:58:20.349+0000", "64.5.234.104", "Internet Explorer", "Indonesian",
		"About John Howard, and with whom he died in combat at the Battle of Bosworth FieAbout "
			"Gerald Ford, cy, Ford served nearly",
		121},
	{(uint64_t)17592186044868, (uint64_t)8246340395593, (uint64_t)4,
		(uint64_t)12749, (uint64_t)2199044220809,
		"", "2010-09-16T03:58:57.294+0000", "190.102.229.236", "Opera", "Kotokoli", "About Sonia "
			"Gandhi, y from politics amAbout Gerald Ford, t of South VietnamAbout Dona",
		85},
	{(uint64_t)4398046568165, (uint64_t)7696581409689, (uint64_t)38,
		(uint64_t)1119, (uint64_t)5497578246736,
		"", "2011-10-22T15:57:54.382+0000", "1.118.78.117", "Chrome", "Kurdish", "About Prince Philip, "
			"Duke of Edinburgh, consort anAbout Kamal Haasan, film produAbout Joe Strummer,  ",
		101},
	{(uint64_t)15393162827013, (uint64_t)1649273819714, (uint64_t)42,
		(uint64_t)9153, (uint64_t)7696585055756,
		"", "2012-06-20T01:37:33.689+0000", "196.43.214.219", "Firefox", "Macedonian", "About Pyotr "
			"Ilyich Tchaikovsky, musical career in Russia at that time, and no system of public music educ",
		105},
	{(uint64_t)15393162854260, (uint64_t)6597077119760, (uint64_t)29,
		(uint64_t)12590, (uint64_t)6597084317456,
		"", "2012-02-08T11:58:29.372+0000", "1.207.116.89", "Firefox", "Marathi", "About Lleyton "
			"Hewitt,  on its list of the 40About Marlene Dietrich, y 1992) was a German-AAbout Pope Pius",
		105},
	{(uint64_t)15393162816482, (uint64_t)3298534953912, (uint64_t)0,
		(uint64_t)537, (uint64_t)2748802952391,
		"", "2010-12-22T02:43:34.560+0000", "119.148.100.227", "Chrome", "Marshallese", "About "
			"Augustine of Hippo, e is also considered a saint, hiAbout Walt Whitman, y 31, 1819 – Ma",
		93},
	{(uint64_t)2199023270266, (uint64_t)6047315028681, (uint64_t)32,
		(uint64_t)14109, (uint64_t)7146846871028,
		"", "2012-04-30T14:43:33.925+0000", "110.232.85.6", "Firefox", "Mirpuri", "About Antonín Dvořák, "
			"ber music, concerti, operas and many of other orchestral and vocal-instrumental pieces. His be",
		116},
	{(uint64_t)21990232571007, (uint64_t)7146826700314, (uint64_t)12,
		(uint64_t)13881, (uint64_t)7696592854031,
		"", "2012-05-09T00:29:11.830+0000", "41.231.186.185", "Firefox", "Nepali", "About Imelda "
			"Marcos, r husband to political power, she heAbout Claude Debussy, he most famous and "
			"influential of alAbout British Empire, and economic tensions between BritaiAb",
		175},
	{(uint64_t)15393162847058, (uint64_t)6047320338608, (uint64_t)24,
		(uint64_t)13264, (uint64_t)4947821641075,
		"", "2011-07-25T22:05:34.484+0000", "217.21.39.183", "Internet Explorer", "Nigerian", "About "
			"Franz Liszt, onductor. He was a benefactor to other composers, including Richard Wagner, Hecto",
		100},
	{(uint64_t)32985348858567, (uint64_t)6047315047104, (uint64_t)108,
		(uint64_t)14586, (uint64_t)1099517717225,
		"", "2010-06-27T03:14:21.051+0000", "41.77.24.111", "Firefox", "Norwegian", "About Plato,  "
			"foundations of Western philosophy and science. In thAbout Mahmoud Abbas, hat second deadlin",
		105},
	{(uint64_t)4398046515646, (uint64_t)4398048701163, (uint64_t)40,
		(uint64_t)12994, (uint64_t)2199050620329,
		"", "2010-09-14T17:30:27.294+0000", "198.58.12.53", "Firefox", "Portuguese", "About Adolf "
			"Hitler, o, in his view, betrayed it. The 1920s, during which his early politAbout Sonia "
			"Gandhi, of India. She is the widow of former Prime Minister of India, Rajiv About Barack "
			"Obama,  Afford",
		203},
	{(uint64_t)2199023323151, (uint64_t)4398050803621, (uint64_t)73,
		(uint64_t)12416, (uint64_t)8246375579396,
		"", "2012-08-04T18:05:20.871+0000", "31.31.170.165", "Chrome", "Romanian", "About Henri "
			"Matisse,  media throughoutAbout Jawaharlal Nehru,  epithet of PandiAbout Ehud Olmert",
		96},
	{(uint64_t)8796093030527, (uint64_t)2748782249873, (uint64_t)7,
		(uint64_t)14001, (uint64_t)3298541028849,
		"", "2011-01-25T19:31:24.130+0000", "165.182.148.17", "Chrome", "Russian", "About Augustine "
			"of Hippo, : Aurelius Augustinus Hipponensis; November 13, 354 – August 28, 430), also "
			"known as Augustine, St. Augustine, About Pope John XXIII, elected on 28 October 1958. "
			"He called the Second Vatican Coun",
		221},
	{(uint64_t)4398046536702, (uint64_t)7696585670555, (uint64_t)96,
		(uint64_t)14936, (uint64_t)8246358120308,
		"", "2012-08-27T08:41:20.431+0000", "1.1.20.159", "Firefox", "Serbian", "About Wolfgang "
			"Amadeus Mozart, he age of five and performed before European royalty.",
		84},
	{(uint64_t)32985348863308, (uint64_t)6047317151225, (uint64_t)61,
		(uint64_t)15063, (uint64_t)549764254109,
		"", "2010-04-22T01:32:29.528+0000", "203.81.162.144", "Firefox", "Shanghainese", "About "
			"Mobutu Sese Seko, an authoritarian regime, amassed vast personal wealth, and attempted "
			"to purge the country of",
		116},
	{(uint64_t)2199023283094, (uint64_t)6597072915904, (uint64_t)75,
		(uint64_t)3652, (uint64_t)8246376547667,
		"", "2012-07-10T20:35:15.529+0000", "60.244.113.24", "Internet Explorer", "Swedish", "About "
			"Richard E. Grant, Richard Esterhuysen; 5 May 1957) is a Swaziland-born British actor, screenw",
		99},
	{(uint64_t)4398046553309, (uint64_t)6597074002780, (uint64_t)77,
		(uint64_t)8042, (uint64_t)3848295027609,
		"", "2011-03-25T15:49:07.998+0000", "31.131.180.6", "Firefox", "Tamil", "About Augustine of "
			"Hippo, Hipponensis; November 13, 3About Dolly Parton, of all time; with an estimaAbout "
			"Kurt Cobain, n, however, was often uncomAbout Nanc",
		156},
	{(uint64_t)17592186070889, (uint64_t)6047317167770, (uint64_t)10,
		(uint64_t)9664, (uint64_t)7696601707824,
		"", "2012-05-11T11:15:17.943+0000", "193.194.3.198", "Firefox", "Telugu", "About Hassan II "
			"of Morocco, 9. He was the eldest sAbout Roy Orbison and Friends, A Bla",
		86},
	{(uint64_t)2199023280088, (uint64_t)8246343522006, (uint64_t)5,
		(uint64_t)14739, (uint64_t)6047345231756,
		"", "2012-01-02T21:08:21.095+0000", "78.157.59.94", "Chrome", "Thai", "About Sanath "
			"Jayasuriya, was an all-rounder, who hadAbout Timbaland, s 1997 album Supa Dupa Fly,About Sa",
		104},
	{(uint64_t)13194139584639, (uint64_t)3848290759875, (uint64_t)17,
		(uint64_t)4942, (uint64_t)2199055393054,
		"", "2010-09-29T23:33:46.438+0000", "46.29.28.224", "Firefox", "Tibetan", "About Rupert "
			"Murdoch, owed closely by The Sun. He moved to New York in 1974 to expand into the US "
			"market and became a na",
		120},
	{(uint64_t)28587302386812, (uint64_t)6047317130630, (uint64_t)68,
		(uint64_t)413, (uint64_t)4947819850477,
		"", "2011-08-03T01:35:57.201+0000", "27.47.198.11", "Firefox", "Turkish", "About Augustine "
			"of Hippo, , the day of his death. He is the patron saint of brewerAbout William McKinley, iam M",
		111},
	{(uint64_t)29060, (uint64_t)6047319282415, (uint64_t)65,
		(uint64_t)6272, (uint64_t)1099527383525,
		"", "2010-06-04T13:35:38.984+0000", "89.164.148.84", "Chrome", "Ukrainian", "About Rubén "
			"Blades, anamanians considerAbout Estonia, nnish. One distinctAbout Ob-La-Di, Ob-La-Da,  "
			"Lennon–McCart",
		113},
	{(uint64_t)15393162811721, (uint64_t)8246337260136, (uint64_t)84,
		(uint64_t)12689, (uint64_t)8246343068836,
		"", "2012-08-12T08:29:06.398+0000", "92.53.55.101", "Opera", "Urdu", "About Edward I of England, "
			"gland pacified, Edward left on a crusade to the Holy Land. The crusade accom",
		103},
	{(uint64_t)15393162859562, (uint64_t)2199028532574, (uint64_t)25,
		(uint64_t)8050, (uint64_t)5497560629768,
		"", "2011-10-18T14:51:16.595+0000", "1.2.2.215", "Firefox", "Uzbek", "About Garry Kasparov, "
			"s Champion in 1985 at the age of 22 by dAbout Holla at Me, . the Album, produced by Cool & Dre.",
		117},
	{(uint64_t)8796093074182, (uint64_t)8246337212075, (uint64_t)78,
		(uint64_t)6342, (uint64_t)549784433132,
		"", "2010-04-28T21:24:18.380+0000", "61.95.246.55", "Chrome", "Vietnamese", "About Edvard "
			"Munch, chological themes built upAbout David Letterman,  follow-up The Late Late SAbout L",
		102},
	{(uint64_t)56628, (uint64_t)8796097266898, (uint64_t)1,
		(uint64_t)494, (uint64_t)7146864801453,
		"", "2012-05-08T19:42:37.828+0000", "36.253.117.87", "Chrome", "Chinese", "About Guy "
			"Sebastian, ustralian acts. Four of his singles have reached multi-platinum certification, "
			"with Angels Brought Me Here, Who's Th",
		137}};

	std::vector<double> runtimes(parameters.size());

	auto expr = create_is6_query();

	auto jit_start = std::chrono::system_clock::now();
	qeng.generate(jit, expr, false);
	auto jit_end = std::chrono::system_clock::now();
	auto comp = std::chrono::duration_cast<std::chrono::microseconds>(jit_end -
			jit_start).count();

	for(auto i = 0u; i < parameters.size(); i++) {
		auto params = parameters[i];
		result_set rs;

		properties_t n = {{"id", boost::any(boost::get<uint64_t>(params[4]))}, 
			{"imageFile", boost::any(boost::get<std::string &>(params[5]))},
			{"creationDate", boost::any(boost::get<std::string &>(params[6]))},
			{"locationIP", boost::any(boost::get<std::string &>(params[7]))},
			{"browserUsed", boost::any(boost::get<std::string &>(params[8]))},
			{"language", boost::any(boost::get<std::string &>(params[9]))}, 
			{"content", boost::any(boost::get<std::string &>(params[10]))},
			{"length", boost::any(boost::get<int>(params[11]))} };
		auto args = create_args_q6();
		args.arg(2, n);
		*args.args.data()[13] = boost::get<uint64_t>(params[0]);
		*args.args.data()[16] = boost::get<uint64_t>(params[1]);
		*args.args.data()[19] = boost::get<uint64_t>(params[2]);
		*args.args.data()[22] = boost::get<uint64_t>(params[3]);
		qeng.prepare(jit, gdb);
		auto start_qp = std::chrono::steady_clock::now();

		auto tx = gdb->begin_transaction();
		qeng.start_[0](gdb.get(), 0, gdb->get_nodes()->num_chunks(), tx, 1, &qeng.type_vec_[0], &rs, nullptr, qeng.finish_[0], 0, args.args.data());
		gdb->commit_transaction();

		auto end_qp = std::chrono::steady_clock::now();
		runtimes[i] = std::chrono::duration_cast<std::chrono::microseconds>(end_qp -
				start_qp).count();
		std::cout << rs << std::endl;
		std::cout << "Runtime: " << runtimes[i] << std::endl;
		joiner::rhs_input_.clear();
	}
	qeng.cleanup();
	return {comp, calc_avg(runtimes)};


}

std::pair<double,double> run_ldbc_is7(JitFromScratch &jit, graph_db_ptr gdb, query_engine & qeng) {
	std::vector<params_tuple> parameters =
	{{(uint64_t)29060, (uint64_t)2748794348006, (uint64_t)55,
		 (uint64_t)14519, (uint64_t)3848295150137,
		 "2011-03-08T14:30:47.923+0000", "60.243.122.214", "Firefox", "duh",
		 3},
	{(uint64_t)15393162813786, (uint64_t)549779918516, (uint64_t)75,
		(uint64_t)12590, (uint64_t)2199024752192,
		"2010-10-05T17:18:38.712+0000", "1.4.7.28", "Firefox", "About Karl Marx, "
			"ampaigned for sociaAbout Léopold Sédar Senghor, r two decades serveAbout ",
		90},
	{(uint64_t)10995116329599, (uint64_t)8246346717801, (uint64_t)96,
		(uint64_t)10171, (uint64_t)7696607996820,
		"2012-06-07T13:17:09.454+0000", "31.211.82.176", "Firefox", "no",
		2},
	{(uint64_t)6597069777270, (uint64_t)1099520146240, (uint64_t)31,
		(uint64_t)4361, (uint64_t)2199046154149,
		"2010-11-01T03:44:38.424+0000", "14.99.62.99", "Firefox", "I see",
		5},
	{(uint64_t)21990232617301, (uint64_t)7146826071966, (uint64_t)77,
		(uint64_t)7774, (uint64_t)3848291205536,
		"2011-03-10T18:32:58.294+0000", "27.109.13.138", "Firefox", "About "
			"Mahmud of Ghazni, s also the first ruler to carry the title About Mali Empir",
		82},
	{(uint64_t)19791209302584, (uint64_t)1649290788264, (uint64_t)51,
		(uint64_t)11566, (uint64_t)7696606699247,
		"2012-05-13T22:49:08.635+0000", "46.23.63.122", "Opera", "About Juan "
			"Carlos I of Spain, sh Constitution of 1978, voAbout Kiribati, dom in",
		79},
	{(uint64_t)15393162816482, (uint64_t)3298545944816, (uint64_t)66,
		(uint64_t)14739, (uint64_t)5497567464709,
		"2011-10-16T17:38:50.408+0000", "122.248.107.11", "Internet Explorer", "About "
			"George Frideric Handel, eatest composers of all time, About Björk, s and Universal ",
		89},
	{(uint64_t)15393162854987, (uint64_t)2199028617158, (uint64_t)58,
		(uint64_t)6901, (uint64_t)1649272452958,
		"2010-09-03T08:12:18.645+0000", "61.17.39.170", "Firefox", "thx",
		3},
	{(uint64_t)32985348862047, (uint64_t)8246368817736, (uint64_t)26,
		(uint64_t)14001, (uint64_t)5497590127542,
		"2011-10-05T14:08:27.891+0000", "46.229.173.1", "Internet Explorer", "thanks",
		6},
	{(uint64_t)32985348891281, (uint64_t)3298555127377, (uint64_t)85,
		(uint64_t)1936, (uint64_t)8246356114707,
		"2012-08-21T08:48:47.468+0000", "27.99.202.62", "Firefox", "ok",
		2},
	{(uint64_t)10995116309820, (uint64_t)7696592322933, (uint64_t)70,
		(uint64_t)8887, (uint64_t)8246347801427,
		"2012-07-15T04:34:07.583+0000", "148.212.83.182", "Firefox", "LOL",
		3},
	{(uint64_t)24189255855187, (uint64_t)4398076428831, (uint64_t)62,
		(uint64_t)831, (uint64_t)4398062080388,
		"2011-05-08T06:36:59.664+0000", "27.121.101.52", "Chrome", "thx",
		3},
	{(uint64_t)28587302365986, (uint64_t)8246375019652, (uint64_t)25,
		(uint64_t)11741, (uint64_t)4398063864847,
		"2011-06-17T23:16:00.439+0000", "88.197.23.147", "Chrome", "About "
			"Celine Dion, g 175 million in album sales worldwide, she was presented "
			"with the Chopard Diamond AwardAbout Unia, ressive metal styles than "
			"their previous work. This is ",
		174},
	{(uint64_t)4398046565737, (uint64_t)6047347702677, (uint64_t)64,
		(uint64_t)1917, (uint64_t)6597075816005,
		"2012-02-22T21:57:59.374+0000", "24.40.38.152", "Chrome", "good",
		4},
	{(uint64_t)32985348893000, (uint64_t)1649283216261, (uint64_t)53,
		(uint64_t)10641, (uint64_t)7696610109460,
		"2012-07-04T08:47:38.365+0000", "27.123.216.178", "Firefox", "no way!",
		7},
	{(uint64_t)32985348891012, (uint64_t)3848300262394, (uint64_t)79,
		(uint64_t)13212, (uint64_t)6047321587911,
		"2011-11-23T13:13:23.454+0000", "27.50.16.88", "Internet Explorer", "right",
		5},
	{(uint64_t)13194139574726, (uint64_t)4947802562793, (uint64_t)3,
		(uint64_t)15934, (uint64_t)3848319690242,
		"2011-04-29T10:28:24.438+0000", "118.103.232.181", "Firefox", "right",
		5},
	{(uint64_t)17592186076766, (uint64_t)4947839161785, (uint64_t)101,
		(uint64_t)543, (uint64_t)7696613005703,
		"2012-06-11T04:46:40.023+0000", "186.96.251.218", "Firefox", "thx",
		3},
	{(uint64_t)10995116279390, (uint64_t)7696591494819, (uint64_t)84,
		(uint64_t)5748, (uint64_t)1649268544415,
		"2010-08-31T06:57:15.119+0000", "110.44.126.182", "Safari", "ok",
		2},
	{(uint64_t)28587302347895, (uint64_t)5497568002951, (uint64_t)11,
		(uint64_t)15062, (uint64_t)4398047536267,
		"2011-06-27T18:06:02.990+0000", "49.1.69.130", "Firefox", "no",
		2},
	{(uint64_t)2199023258582, (uint64_t)6047334641134, (uint64_t)78,
		(uint64_t)12405, (uint64_t)7146829588243,
		"2012-03-11T18:08:21.123+0000", "46.253.228.240", "Chrome", "thx",
		3},
	{(uint64_t)17592186096316, (uint64_t)5497570566613, (uint64_t)10,
		(uint64_t)6880, (uint64_t)6047351780580,
		"2012-01-05T19:53:09.593+0000", "61.0.45.102", "Internet Explorer", "no way!",
		7},
	{(uint64_t)19791209357939, (uint64_t)7146854943821, (uint64_t)81,
		(uint64_t)15001, (uint64_t)3848316169304,
		"2011-03-31T05:33:20.748+0000", "14.144.70.240", "Internet Explorer", "ok",
		2},
	{(uint64_t)40261, (uint64_t)7696600185408, (uint64_t)34,
		(uint64_t)11868, (uint64_t)7696599178697,
		"2012-05-13T08:50:32.249+0000", "14.102.140.203", "Chrome", "About Julie "
			"Andrews, he 1990s. Andrews' film career had a major revival in the 2000s with ",
		90},
	{(uint64_t)8796093030527, (uint64_t)4398076275780, (uint64_t)1,
		(uint64_t)11101, (uint64_t)7146860632721,
		"2012-04-05T06:16:56.566+0000", "192.160.128.188", "Firefox", "fine",
		4},
	{(uint64_t)19791209365081, (uint64_t)8246375086192, (uint64_t)82,
		(uint64_t)14391, (uint64_t)4947836042392,
		"2011-07-14T06:22:30.543+0000", "31.41.255.70", "Firefox", "About Ernest "
			"Hemingway,  Civil War where he had acted as a journalist, and after which he",
		89},
	{(uint64_t)26388279136569, (uint64_t)8246361195572, (uint64_t)99,
		(uint64_t)7398, (uint64_t)3298550508017,
		"2011-02-03T02:09:40.466+0000", "61.11.45.19", "Internet Explorer", "About "
			"Robert Schumann, ft für Musik (NAbout Jimmy Page, glish guitaristAbout Ukraine, a",
		87},
	{(uint64_t)30786325642083, (uint64_t)5497588536402, (uint64_t)73,
		(uint64_t)7063, (uint64_t)6597081994683,
		"2012-02-21T02:33:50.333+0000", "31.209.93.42", "Chrome", "About Woody Allen, "
			"eloped as a standup. The best-known of his ovAbout Hell-O,  produced i",
		89},
	{(uint64_t)17592186088378, (uint64_t)3848330739206, (uint64_t)67,
		(uint64_t)11106, (uint64_t)6047315757604,
		"2011-12-22T00:41:25.308+0000", "49.45.140.197", "Firefox", "right",
		5},
	{(uint64_t)13194139562905, (uint64_t)4947803668797, (uint64_t)5,
		(uint64_t)13326, (uint64_t)5497573645899,
		"2011-10-19T21:43:49.914+0000", "101.211.149.203", "Firefox", "About Garry "
			"Kasparov, ep Blue in 19About Nero, Tacitus, SuetAbout Neil Gai",
		74},
	{(uint64_t)21990232576546, (uint64_t)5497569938768, (uint64_t)90,
		(uint64_t)2260, (uint64_t)6047316223314,
		"2011-12-29T07:06:39.440+0000", "2.209.63.64", "Internet Explorer", "fine",
		4},
	{(uint64_t)4398046574041, (uint64_t)8246350963093, (uint64_t)61,
		(uint64_t)13401, (uint64_t)7146846361437,
		"2012-04-28T18:12:11.293+0000", "31.207.40.63", "Internet Explorer", "cool",
		4},
	{(uint64_t)15393162817739, (uint64_t)3298548851836, (uint64_t)88,
		(uint64_t)14700, (uint64_t)2199024789469,
		"2010-09-14T23:40:42.006+0000", "202.69.10.5", "Chrome", "About Sonia Gandhi, "
			"way from politAbout Hungary, ear (2007). ThAbout Chile, and",
		79},
	{(uint64_t)26388279089559, (uint64_t)5497592998627, (uint64_t)30,
		(uint64_t)12253, (uint64_t)3298539678828,
		"2011-01-04T06:00:06.364+0000", "14.102.233.80", "Internet Explorer", "ok",
		2},
	{(uint64_t)13194139603906, (uint64_t)8246352436549, (uint64_t)103,
		(uint64_t)6822, (uint64_t)7146845506463,
		"2012-04-21T04:14:32.940+0000", "61.247.234.57", "Internet Explorer", "no way!",
		7},
	{(uint64_t)4398046515646, (uint64_t)7696593871297, (uint64_t)6,
		(uint64_t)13088, (uint64_t)6047350073324,
		"2011-12-05T02:38:04.702+0000", "49.246.208.136", "Opera", "roflol",
		6},
	{(uint64_t)21990232571007, (uint64_t)5497561308127, (uint64_t)60,
		(uint64_t)6272, (uint64_t)8246353108187,
		"2012-08-26T00:03:24.818+0000", "46.31.114.181", "Chrome", "About Ludwig "
			"van Beethoven, us and influential of all composers. Born in Bonn, the",
		82},
	{(uint64_t)21990232583408, (uint64_t)2199032464941, (uint64_t)87,
		(uint64_t)537, (uint64_t)3298552886798,
		"2011-02-16T18:37:46.095+0000", "41.87.201.129", "Internet Explorer", "About "
			"Allied-occupied Germany, ), although the need foAbout Insane in the Brain, ber 1 on ",
		90},
	{(uint64_t)2199023292039, (uint64_t)7146829550853, (uint64_t)69,
		(uint64_t)6338, (uint64_t)7146833473876,
		"2012-05-08T19:47:46.279+0000", "8.183.234.123", "Chrome", "About Che Guevara,  "
			"collective imAbout We Are Golden, e Golden is thAbout Like a ",
		81},
	{(uint64_t)6597069782503, (uint64_t)5497561169934, (uint64_t)33,
		(uint64_t)1753, (uint64_t)3848314221503,
		"2011-03-15T10:47:39.393+0000", "196.1.94.212", "Internet Explorer", "no way!",
		7},
	{(uint64_t)19791209339396, (uint64_t)6047350833590, (uint64_t)8,
		(uint64_t)13327, (uint64_t)5497558701786,
		"2011-10-22T16:53:01.533+0000", "49.206.101.97", "Firefox", "fine",
		4},
	{(uint64_t)2199023256300, (uint64_t)3848309363368, (uint64_t)14,
		(uint64_t)7619, (uint64_t)5497567100845,
		"2011-10-15T15:55:59.260+0000", "213.55.65.113", "Chrome", "great",
		5},
	{(uint64_t)32985348833794, (uint64_t)5497591966052, (uint64_t)92,
		(uint64_t)15002, (uint64_t)5497558520926,
		"2011-09-23T05:41:28.752+0000", "103.3.78.253", "Firefox", "About "
			"Wilhelm II, German Emperor, e was humiliated by the Daily Telegraph affaAbout Pa",
		86},
	{(uint64_t)26388279115622, (uint64_t)7146856113593, (uint64_t)13,
		(uint64_t)6230, (uint64_t)3848317880036,
		"2011-04-17T22:06:47.877+0000", "27.116.30.141", "Internet Explorer", "duh",
		3},
	{(uint64_t)8796093045679, (uint64_t)2748804425261, (uint64_t)76,
		(uint64_t)4346, (uint64_t)5497563730633,
		"2011-10-13T01:36:55.405+0000", "103.3.70.39", "Internet Explorer", "fine",
		4},
	{(uint64_t)15393162811721, (uint64_t)7696617056842, (uint64_t)108,
		(uint64_t)9822, (uint64_t)7696602664590,
		"2012-05-12T12:02:23.286+0000", "81.28.55.46", "Chrome", "About Charles VI, "
			"Holy Roman Emperor, ctober 1740) was the About Gaetano Donizett",
		81},
	{(uint64_t)2199023282192, (uint64_t)3298572083899, (uint64_t)91,
		(uint64_t)492, (uint64_t)7146827132717,
		"2012-04-24T05:47:32.217+0000", "1.2.10.134", "Firefox", "fine",
		4},
	{(uint64_t)4398046543803, (uint64_t)5497580119697, (uint64_t)7,
		(uint64_t)874, (uint64_t)1099535287122,
		"2010-05-27T11:40:12.337+0000", "58.146.119.69", "Chrome", "no way!",
		7},
	{(uint64_t)19791209335491, (uint64_t)7146865987740, (uint64_t)94,
		(uint64_t)7399, (uint64_t)6597071911040,
		"2012-02-21T09:01:17.401+0000", "61.11.115.18", "Firefox", "About Steven "
			"Spielberg,  one of the co-foundersAbout Muddy Waters, osion in the 1960s, an",
		89},
	{(uint64_t)13194139605772, (uint64_t)2748796300522, (uint64_t)37,
		(uint64_t)11015, (uint64_t)6597095313704,
		"2012-02-21T10:47:28.864+0000", "202.14.70.59", "Chrome", "About Joan of Arc, "
			"léans as part ofAbout Sukarno, orn Kusno SosrodAbout Stephen Fry,  As well as "
			"his About John Dryden, t",
		121},
	{(uint64_t)26388279139007, (uint64_t)4947828404741, (uint64_t)95,
		(uint64_t)6342, (uint64_t)549783165349,
		"2010-04-28T23:14:00.477+0000", "119.252.224.122", "Chrome", "I see",
		5},
	{(uint64_t)13194139551277, (uint64_t)8246340867513, (uint64_t)19,
		(uint64_t)2537, (uint64_t)8246350127979,
		"2012-08-04T09:52:41.688+0000", "110.35.65.18", "Firefox", "thanks",
		6}};
	std::vector<double> runtimes(parameters.size());

	auto expr = create_is7_query();

	auto jit_start = std::chrono::system_clock::now();
	qeng.generate(jit, expr, false);
	auto jit_end = std::chrono::system_clock::now();
	auto comp = std::chrono::duration_cast<std::chrono::microseconds>(jit_end -
			jit_start).count();

	for(auto i = 0u; i < parameters.size(); i++) {
		auto params = parameters[i];
		result_set rs;

		properties_t n = {{"id", boost::any(boost::get<uint64_t>(params[4]))},
			{"creationDate", boost::any(boost::get<std::string &>(params[5]))},
			{"locationIP", boost::any(boost::get<std::string &>(params[6]))},
			{"browserUsed", boost::any(boost::get<std::string &>(params[7]))},
			{"content", boost::any(boost::get<std::string &>(params[8]))},
			{"length", boost::any(boost::get<int>(params[9]))} };
		auto args = create_args_q7();
		args.arg(2, n);
		*args.args.data()[13] = boost::get<uint64_t>(params[0]);
		*args.args.data()[16] = boost::get<uint64_t>(params[1]);
		*args.args.data()[19] = boost::get<uint64_t>(params[2]);
		*args.args.data()[22] = boost::get<uint64_t>(params[3]);
		qeng.prepare(jit, gdb);
		auto start_qp = std::chrono::steady_clock::now();

		auto tx = gdb->begin_transaction();
		qeng.start_[0](gdb.get(), 0, gdb->get_nodes()->num_chunks(), tx, 1, &qeng.type_vec_[0], &rs, nullptr, qeng.finish_[0], 0, args.args.data());
		gdb->commit_transaction();

		auto end_qp = std::chrono::steady_clock::now();
		runtimes[i] = std::chrono::duration_cast<std::chrono::microseconds>(end_qp -
				start_qp).count();
		std::cout << rs << std::endl;
		std::cout << "Runtime: " << runtimes[i] << std::endl;
		joiner::rhs_input_.clear();
	}
	qeng.cleanup();
	return {comp, calc_avg(runtimes)};

}

std::pair<double,double> run_ldbc_is8(JitFromScratch &jit, graph_db_ptr gdb, query_engine & qeng) {
	std::vector<params_tuple> parameters =
	{{(uint64_t)6597069814754, (uint64_t)6597069805171, "2010-07-21T10:45:29.157+0000"},
		{(uint64_t)15393162848091, (uint64_t)15393162811335, "2011-04-06T14:31:57.569+0000"},
		{(uint64_t)13194139539462, (uint64_t)10995116300493, "2011-03-27T09:02:50.140+0000"},
		{(uint64_t)13194139565132, (uint64_t)13194139564977, "2011-03-05T01:39:38.306+0000"},
		{(uint64_t)21990232577108, (uint64_t)19791209372566, "2011-10-31T13:18:47.590+0000"},
		{(uint64_t)19791209332467, (uint64_t)19791209328998, "2011-08-31T19:40:46.196+0000"},
		{(uint64_t)8796093082949, (uint64_t)8796093078792, "2010-11-19T23:39:54.268+0000"},
		{(uint64_t)10995116314445, (uint64_t)8796093027771, "2011-01-06T12:35:02.917+0000"},
		{(uint64_t)8796093054613, (uint64_t)8796093050285, "2010-10-26T07:54:15.336+0000"},
		{(uint64_t)4398046541894, (uint64_t)4398046527770, "2010-07-11T20:33:52.401+0000"},
		{(uint64_t)17592186062775, (uint64_t)17592186054539, "2011-06-20T22:22:47.316+0000"},
		{(uint64_t)10995116313202, (uint64_t)10995116307578, "2011-01-26T13:41:21.174+0000"},
		{(uint64_t)17592186074634, (uint64_t)17592186047922, "2011-07-04T09:40:23.682+0000"},
		{(uint64_t)13194139590688, (uint64_t)10995116312769, "2011-02-17T06:21:22.623+0000"},
		{(uint64_t)32985348876742, (uint64_t)32985348864613, "2012-08-27T01:48:24.344+0000"},
		{(uint64_t)21990232593722, (uint64_t)21990232591512, "2011-11-12T22:21:47.854+0000"},
		{(uint64_t)2199023263949, (uint64_t)45800, "2010-04-22T23:37:11.373+0000"},
		{(uint64_t)15393162796675, (uint64_t)10995116325522, "2011-04-30T02:09:05.705+0000"},
		{(uint64_t)2199023256181, (uint64_t)69708, "2010-04-29T22:23:20.266+0000"},
		{(uint64_t)21990232627050, (uint64_t)21990232599826, "2011-11-23T12:58:03.731+0000"},
		{(uint64_t)13194139547995, (uint64_t)13194139539971, "2011-02-26T19:42:07.211+0000"},
		{(uint64_t)4398046544907, (uint64_t)4398046542609, "2010-06-12T12:22:33.943+0000"},
		{(uint64_t)10995116281711, (uint64_t)10995116280688, "2010-12-03T22:01:21.190+0000"},
		{(uint64_t)32985348840058, (uint64_t)30786325648650, "2012-08-10T03:32:45.050+0000"},
		{(uint64_t)24189255829155, (uint64_t)17592186084880, "2011-11-30T17:12:13.272+0000"},
		{(uint64_t)19791209302521, (uint64_t)19791209301374, "2011-08-15T10:35:00.845+0000"},
		{(uint64_t)6597069807691, (uint64_t)6597069806535, "2010-09-26T16:54:37.902+0000"},
		{(uint64_t)32985348840352, (uint64_t)30786325600599, "2012-09-09T07:37:20.283+0000"},
		{(uint64_t)21990232623366, (uint64_t)21990232591519, "2011-11-09T05:47:40.792+0000"},
		{(uint64_t)17592186078543, (uint64_t)17592186078051, "2011-08-02T21:01:53.496+0000"},
		{(uint64_t)30786325622518, (uint64_t)26388279083639, "2012-06-25T20:40:35.550+0000"},
		{(uint64_t)10995116332467, (uint64_t)10995116328236, "2011-01-12T09:30:34.311+0000"},
		{(uint64_t)17592186083857, (uint64_t)15393162810464, "2011-05-28T09:40:34.855+0000"},
		{(uint64_t)4398046523681, (uint64_t)4398046522623, "2010-07-12T06:11:41.127+0000"},
		{(uint64_t)2199023283486, (uint64_t)2199023282647, "2010-04-06T21:32:01.736+0000"},
		{(uint64_t)13194139545357, (uint64_t)13194139539264, "2011-03-19T12:05:46.844+0000"},
		{(uint64_t)6597069805301, (uint64_t)6597069802419, "2010-08-15T19:10:52.010+0000"},
		{(uint64_t)17233, (uint64_t)6749, "2010-03-02T20:52:19.747+0000"},
		{(uint64_t)21990232580825, (uint64_t)21990232572198, "2011-10-25T17:24:07.637+0000"},
		{(uint64_t)8796093092197, (uint64_t)8796093091841, "2010-11-28T08:41:16.266+0000"},
		{(uint64_t)48245, (uint64_t)41266, "2010-02-12T20:19:53.503+0000"},
		{(uint64_t)24189255827326, (uint64_t)24189255823054, "2011-12-02T02:12:34.844+0000"},
		{(uint64_t)24189255849688, (uint64_t)19791209317656, "2011-11-25T02:58:53.510+0000"},
		{(uint64_t)10995116333663, (uint64_t)10995116306062, "2010-12-27T02:54:54.100+0000"},
		{(uint64_t)4398046527355, (uint64_t)4398046527192, "2010-06-14T21:51:44.216+0000"},
		{(uint64_t)28587302338242, (uint64_t)28587302335765, "2012-05-31T13:19:37.937+0000"},
		{(uint64_t)30786325612907, (uint64_t)28587302346651, "2012-07-19T13:57:38.620+0000"},
		{(uint64_t)6597069804976, (uint64_t)2199023320857, "2010-08-04T16:09:53.448+0000"},
		{(uint64_t)24189255880599, (uint64_t)24189255840594, "2011-12-24T02:45:50.361+0000"},
		{(uint64_t)19791209310595, (uint64_t)19791209309631, "2011-09-07T13:57:04.118+0000"},
		{(uint64_t)6597069833663, (uint64_t)2199023273453, "2010-07-15T18:18:51.077+0000"},
		{(uint64_t)17592186087895, (uint64_t)15393162842178, "2011-05-20T15:32:55.041+0000"}};

	std::vector<double> runtimes(parameters.size());

	auto expr = create_is8_query();

	auto jit_start = std::chrono::system_clock::now();
	qeng.generate(jit, expr, false);
	auto jit_end = std::chrono::system_clock::now();
	auto comp = std::chrono::duration_cast<std::chrono::microseconds>(jit_end -
			jit_start).count();

	for(auto i = 0u; i < parameters.size(); i++) {
		auto params = parameters[i];
		result_set rs;

		properties_t n = {{"creationDate", boost::any(boost::get<std::string &>(params[2]))}};
		auto args = create_args_q8();
		args.arg(5, n);
		*args.args.data()[3] = boost::get<uint64_t>(params[1]);
		*args.args.data()[8] = boost::get<uint64_t>(params[0]);
		qeng.prepare(jit, gdb);
		auto start_qp = std::chrono::steady_clock::now();

		auto tx = gdb->begin_transaction();
		qeng.start_[0](gdb.get(), 0, gdb->get_nodes()->num_chunks(), tx, 1, &qeng.type_vec_[0], &rs, nullptr, qeng.finish_[0], 0, args.args.data());
		gdb->commit_transaction();

		auto end_qp = std::chrono::steady_clock::now();
		runtimes[i] = std::chrono::duration_cast<std::chrono::microseconds>(end_qp -
				start_qp).count();
		std::cout << rs << std::endl;
		std::cout << "Runtime: " << runtimes[i] << std::endl;
		joiner::rhs_input_.clear();
	}
	qeng.cleanup();
	return {comp, calc_avg(runtimes)};


}

double internal_is1(graph_db_ptr graph) {

	std::vector<uint64_t> personIds =
	{26388279115622, 2199023280088, 19791209323074, 15393162816482, 15393162855246,
		19791209363124, 28587302347895, 10995116305049, 4398046515646, 2199023282192,
		30786325588998, 2199023301366, 19791209333857, 24189255835727, 26388279139007,
		24189255855187, 17592186088378, 6597069782503, 32985348854773, 21990232576546,
		26388279080215, 21990232627526, 15393162855372, 2199023261717, 10995116348358,
		6597069771464, 17592186084059, 15393162854260, 4398046543803, 71623, 32985348891012,
		6597069786049, 30786325626045, 8796093025994, 8796093041362, 13194139568647,
		28587302324409, 10995116329599, 17592186068239, 30786325621884, 13194139605452,
		4398046511596, 13194139597106, 13194139540307, 32985348862047, 28587302352600,
		17592186070889, 8796093026236, 17592186094267, 32985348870938, 21990232600629,
		2199023283094};
	std::vector<double> runtimes(personIds.size());
	for(auto i = 0u; i < personIds.size(); i++) {
		result_set rs;
		auto q = query(graph)
			.all_nodes("Person")
			.property("id",
					[&](auto &c) { return c.equal(personIds[i]); })
			.from_relationships(":isLocatedIn")
			.to_node("Place")
			.project({PExpr_(0, builtin::string_property(res, "firstName")),
					PExpr_(0, builtin::string_property(res, "lastName")),
					PExpr_(0, builtin::pr_date(res, "birthday")),
					PExpr_(0, builtin::string_property(res, "locationIP")),
					PExpr_(0, builtin::string_property(res, "browserUsed")),
					PExpr_(2, builtin::uint64_property(res, "id")),
					PExpr_(0, builtin::string_property(res, "gender")),
					PExpr_(0, builtin::ptime_property(res, "creationDate")) })
			.collect(rs);

		auto t1 = std::chrono::system_clock::now();
		auto tx = graph->begin_transaction();
		q.start();
		graph->commit_transaction();
		auto t2 = std::chrono::system_clock::now();

		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
		runtimes[i] = duration.count();
	}
	return calc_avg(runtimes);
}

#ifdef USE_PMDK
struct root {
	graph_db_ptr graph;
};
#endif
std::string test_path = "/mnt/pmem0/poseidon/sf10";
std::string db_name = "1";

void load_in_memory(graph_db_ptr &graph);

int main(int argc, char **argv) {
	cl::ParseCommandLineOptions(argc, argv);

	InitLLVM X(argc, argv);
	ExitOnError exitOnError;
	exitOnError.setBanner("JIT");
	InitializeNativeTarget();
	InitializeNativeTargetAsmPrinter();
	InitializeNativeTargetAsmParser();

#ifdef USE_PMDK
	namespace nvm = pmem::obj;
	nvm::pool<root> pop;

	if (access(test_path.c_str(), F_OK) != 0) {
		std::cerr << "Cannot find pmem path '" << test_path << "'" << std::endl;
		return -1;
	} else {
		pop = nvm::pool<root>::open(test_path, db_name);
	}

	auto q = pop.root();
	if (!q->graph) {
		std::cerr << "Cannot open database '" << db_name << "'" << std::endl;
		return -1;
	}

	auto graph = q->graph;
	graph->runtime_initialize();
#else
	auto graph = p_make_ptr<graph_db>(db_name);
	load_in_memory(graph);
#endif

	//auto graph = graph1();
	Collector::gdb = graph.get();

	PContext ctx(graph);
	JitFromScratch Jit(exitOnError);
	ctx.getModule().setDataLayout(Jit.getDataLayout());

	auto THREAD_NUM = 4;
	auto chunks = graph->get_nodes()->num_chunks();
	auto cv_range = chunks / THREAD_NUM;

	query_engine queryEngine(ctx, THREAD_NUM, cv_range);

	result_set rs2;


	queryEngine.cleanup();
	auto is1 = run_ldbc_is1(Jit, graph, queryEngine);
	std::cout << "IS1 compilation: " << is1.first << " runtime: " << is1.second << std::endl;

	auto is2 = run_ldbc_is2(Jit, graph, queryEngine);
	std::cout << "IS2 compilation: " << is2.first << " runtime: " << is2.second << std::endl;

	auto is3 = run_ldbc_is3(Jit, graph, queryEngine);
	std::cout << "IS3 compilation: " << is3.first << " runtime: " << is3.second << std::endl;

	auto is4 = run_ldbc_is4(Jit, graph, queryEngine);
	std::cout << "IS4 compilation: " << is4.first << " runtime: " << is4.second << std::endl;

	auto is5 = run_ldbc_is5(Jit, graph, queryEngine);
	std::cout << "IS5 compilation: " << is5.first << " runtime: " << is5.second << std::endl;

	auto is6 = run_ldbc_is6(Jit, graph, queryEngine);
	std::cout << "IS6 compilation: " << is6.first << " runtime: " << is6.second << std::endl;

	auto is7 = run_ldbc_is7(Jit, graph, queryEngine);
	std::cout << "IS7 compilation: " << is7.first << " runtime: " << is7.second << std::endl;

	auto is8 = run_ldbc_is8(Jit, graph, queryEngine);
	std::cout << "IS8 compilation: " << is8.first << " runtime: " << is8.second << std::endl;


	//auto is1int = internal_is1(graph);
	//std::cout << "IS1 internal: " << is1int << std::endl;

	//expr->codegen(iv, 1, true);
	scan_task::callee_ = &scan_task::scan;
	namespace pj = builtin;
	result_set rs;
	uint64_t personId = 6597069825699;

	auto q1 = query(graph)
		.all_nodes("Person")
		.property("id",
				[&](auto &c) { return c.equal(personId); })
		.from_relationships(":isLocatedIn")
		.to_node("Place")
		.project({PExpr_(0, pj::string_property(res, "firstName")),
				PExpr_(0, pj::string_property(res, "lastName")),
				PExpr_(0, pj::pr_date(res, "birthday")),
				PExpr_(0, pj::string_property(res, "locationIP")),
				PExpr_(0, pj::string_property(res, "browserUsed")),
				PExpr_(2, pj::uint64_property(res, "id")),
				PExpr_(0, pj::string_property(res, "gender")),
				PExpr_(0, pj::ptime_property(res, "creationDate")) })
		.collect(rs);

	auto t7 = std::chrono::system_clock::now();
	auto tx = graph->begin_transaction();
	query::start({&q1});
	graph->commit_transaction();

	auto t8 = std::chrono::system_clock::now();
	std::cout << rs << std::endl;

	//auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
	//printf("Total code generation time: %d\n", duration.count());
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t8 - t7);
	printf("Total internal time: %d\n", duration.count());
	return 0;
}

algebra_optr create_is1_query() {
	result_set rs;
	auto i1 = IndexScan(End());

	auto expr = CreateNode(
			Join(JOIN_OP::CROSS, {},
				CreateRship({0,1},
					Join(JOIN_OP::CROSS, {},
						CreateRship({3, 0},
							Join(JOIN_OP::CROSS, {0,1},
								CreateRship({0,5},
									Join(JOIN_OP::CROSS, {},
										CreateRship({0,7},
											Collect(rs)), i1)), i1)), i1)), i1));
	return expr;
}


algebra_optr create_is2_query() {
	result_set rs;
	auto i1 = IndexScan(End());

	auto expr = IndexScan(
			Join(JOIN_OP::CROSS, {},
				CreateRship({0,1}, Collect(rs)), i1));

	return expr;
}


algebra_optr create_is3_query() {
	result_set rs;
	auto i1 = IndexScan(End());

	auto expr = IndexScan(
			Join(JOIN_OP::CROSS, {},
				CreateRship({0,1}, Collect(rs)), i1));

	return expr;
}

algebra_optr create_is4_query() {
	result_set rs;
	auto id1 = IndexScan(End());

	auto expr = CreateNode(
			Join(JOIN_OP::CROSS, {},
				CreateRship({0,1},
					Join(JOIN_OP::CROSS, {},
						CreateRship({0,3},
							Collect(rs)), id1)), id1));
	return expr;

}

algebra_optr create_is5_query() {
	result_set rs;
	auto id1 = IndexScan(End());

	auto expr = IndexScan(Join(JOIN_OP::CROSS, {},
				CreateRship({0,1}, Collect(rs)), id1));

	return expr;
}

algebra_optr create_is6_query() {
	result_set rs;
	auto i1 = IndexScan(End());

	auto expr = CreateNode(
			Join(JOIN_OP::CROSS, {},
				CreateRship({0,1},
					Join(JOIN_OP::CROSS, {},
						CreateRship({3, 0},
							Join(JOIN_OP::CROSS, {0,1},
								CreateRship({0,5},
									Join(JOIN_OP::CROSS, {},
										CreateRship({0,7},
											Collect(rs)), i1)), i1)), i1)), i1));


	return expr;
}

algebra_optr create_is7_query() {
	result_set rs;
	auto i1 = IndexScan(End());

	auto expr = CreateNode(
			Join(JOIN_OP::CROSS, {},
				CreateRship({0,1},
					Join(JOIN_OP::CROSS, {},
						CreateRship({3, 0},
							Join(JOIN_OP::CROSS, {0,1},
								CreateRship({0,5},
									Join(JOIN_OP::CROSS, {},
										CreateRship({0,7},
											Collect(rs)), i1)), i1)), i1)), i1));


	return expr;
}

algebra_optr create_is8_query() {
	result_set rs;
	auto id1 = IndexScan(End());

	auto expr = IndexScan(Join(JOIN_OP::CROSS, {},
				CreateRship({0,1}, Collect(rs)), id1));

	return expr;
}


void load_snb_data(graph_db_ptr &graph, 
		std::vector<std::string> &node_files,
		std::vector<std::string> &rship_files){
	auto delim = '|';
	graph_db::mapping_t mapping;
	bool nodes_imported = false, rships_imported = false;

	if (!node_files.empty()){
		spdlog::info("######## NODES ########");

		std::vector<std::size_t> num_nodes(node_files.size());
		auto i = 0;
		for (auto &file : node_files){
			std::vector<std::string> fp;
			boost::split(fp, file, boost::is_any_of("/"));
			assert(fp.back().find(".csv") != std::string::npos);
			auto pos = fp.back().find("_");
			auto label = fp.back().substr(0, pos);
			if (label[0] >= 'a' && label[0] <= 'z')
				label[0] -= 32;

			num_nodes[i] = graph->import_nodes_from_csv(label, file, delim, mapping);
			spdlog::info("{} '{}' node objects imported", num_nodes[i], label);
			if (num_nodes[i] > 0)
				nodes_imported = true;
			i++;
		}
	}

	if (!rship_files.empty()){
		spdlog::info("################ RELATIONSHIPS ################");

		std::vector<std::size_t> num_rships(rship_files.size());
		auto i = 0;
		for (auto &file : rship_files){
			std::vector<std::string> fp;
			boost::split(fp, file, boost::is_any_of("/"));
			assert(fp.back().find(".csv") != std::string::npos);
			std::vector<std::string> fn;
			boost::split(fn, fp.back(), boost::is_any_of("_"));
			auto label = ":" + fn[1];

			num_rships[i] = graph->import_relationships_from_csv(file, delim, mapping);
			spdlog::info("{} ({})-[{}]-({}) relationship objects imported", 
					num_rships[i], fn[0], label, fn[2]);
			if (num_rships[i] > 0)
				rships_imported = true;
			i++;
		}
	}
}

void load_in_memory(graph_db_ptr &graph){
	std::string snb_home = 
#ifdef SF_10
		"/home/data/SNB_SF_10/";
#else
	"/home/data/SNB_SF_1/";
#endif

	std::string snb_sta = snb_home + "/static/";
	std::string snb_dyn = snb_home + "/dynamic/";

	std::vector<std::string> node_files = 
	{snb_sta + "place_0_0.csv", snb_sta + "place_1_0.csv",
		snb_sta + "place_2_0.csv", snb_sta + "place_3_0.csv",
		snb_sta + "organisation_0_0.csv", snb_sta + "organisation_1_0.csv",
		snb_sta + "organisation_2_0.csv", snb_sta + "organisation_3_0.csv",
		snb_sta + "tagclass_0_0.csv", snb_sta + "tagclass_1_0.csv",
		snb_sta + "tagclass_2_0.csv", snb_sta + "tagclass_3_0.csv",
		snb_sta + "tag_0_0.csv", snb_sta + "tag_1_0.csv",
		snb_sta + "tag_2_0.csv", snb_sta + "tag_3_0.csv",
		snb_dyn + "comment_0_0.csv", snb_dyn + "comment_1_0.csv",
		snb_dyn + "comment_2_0.csv", snb_dyn + "comment_3_0.csv",
		snb_dyn + "forum_0_0.csv", snb_dyn + "forum_1_0.csv",
		snb_dyn + "forum_2_0.csv", snb_dyn + "forum_3_0.csv",
		snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
		snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
		snb_dyn + "post_0_0.csv", snb_dyn + "post_1_0.csv",
		snb_dyn + "post_2_0.csv", snb_dyn + "post_3_0.csv"};

	std::vector<std::string> rship_files = 
	{snb_dyn + "comment_hasCreator_person_0_0.csv",
		snb_dyn + "comment_hasCreator_person_1_0.csv",
		snb_dyn + "comment_hasCreator_person_2_0.csv",
		snb_dyn + "comment_hasCreator_person_3_0.csv",
		snb_dyn + "comment_isLocatedIn_place_0_0.csv",
		snb_dyn + "comment_isLocatedIn_place_1_0.csv",
		snb_dyn + "comment_isLocatedIn_place_2_0.csv",
		snb_dyn + "comment_isLocatedIn_place_3_0.csv",
		snb_dyn + "comment_replyOf_comment_0_0.csv",
		snb_dyn + "comment_replyOf_comment_1_0.csv",
		snb_dyn + "comment_replyOf_comment_2_0.csv",
		snb_dyn + "comment_replyOf_comment_3_0.csv",
		snb_dyn + "comment_replyOf_post_0_0.csv",
		snb_dyn + "comment_replyOf_post_1_0.csv",
		snb_dyn + "comment_replyOf_post_2_0.csv",
		snb_dyn + "comment_replyOf_post_3_0.csv",
		snb_dyn + "forum_containerOf_post_0_0.csv",
		snb_dyn + "forum_containerOf_post_1_0.csv",
		snb_dyn + "forum_containerOf_post_2_0.csv",
		snb_dyn + "forum_containerOf_post_3_0.csv",
		snb_dyn + "forum_hasMember_person_0_0.csv",
		snb_dyn + "forum_hasMember_person_1_0.csv",
		snb_dyn + "forum_hasMember_person_2_0.csv",
		snb_dyn + "forum_hasMember_person_3_0.csv",
		snb_dyn + "forum_hasModerator_person_0_0.csv",
		snb_dyn + "forum_hasModerator_person_1_0.csv",
		snb_dyn + "forum_hasModerator_person_2_0.csv",
		snb_dyn + "forum_hasModerator_person_3_0.csv",
		snb_dyn + "forum_hasTag_tag_0_0.csv",
		snb_dyn + "forum_hasTag_tag_1_0.csv",
		snb_dyn + "forum_hasTag_tag_2_0.csv",
		snb_dyn + "forum_hasTag_tag_3_0.csv",
		snb_dyn + "person_hasInterest_tag_0_0.csv",
		snb_dyn + "person_hasInterest_tag_1_0.csv",
		snb_dyn + "person_hasInterest_tag_2_0.csv",
		snb_dyn + "person_hasInterest_tag_3_0.csv",
		snb_dyn + "person_isLocatedIn_place_0_0.csv",
		snb_dyn + "person_isLocatedIn_place_1_0.csv",
		snb_dyn + "person_isLocatedIn_place_2_0.csv",
		snb_dyn + "person_isLocatedIn_place_3_0.csv",
		snb_dyn + "person_knows_person_0_0.csv",
		snb_dyn + "person_knows_person_1_0.csv",
		snb_dyn + "person_knows_person_2_0.csv",
		snb_dyn + "person_knows_person_3_0.csv",
		snb_dyn + "person_likes_comment_0_0.csv",
		snb_dyn + "person_likes_comment_1_0.csv",
		snb_dyn + "person_likes_comment_2_0.csv",
		snb_dyn + "person_likes_comment_3_0.csv",
		snb_dyn + "person_likes_post_0_0.csv",
		snb_dyn + "person_likes_post_1_0.csv",
		snb_dyn + "person_likes_post_2_0.csv",
		snb_dyn + "person_likes_post_3_0.csv",
		snb_dyn + "post_hasCreator_person_0_0.csv",
		snb_dyn + "post_hasCreator_person_1_0.csv",
		snb_dyn + "post_hasCreator_person_2_0.csv",
		snb_dyn + "post_hasCreator_person_3_0.csv",
		snb_dyn + "comment_hasTag_tag_0_0.csv",
		snb_dyn + "comment_hasTag_tag_1_0.csv",
		snb_dyn + "comment_hasTag_tag_2_0.csv",
		snb_dyn + "comment_hasTag_tag_3_0.csv",
		snb_dyn + "post_hasTag_tag_0_0.csv",
		snb_dyn + "post_hasTag_tag_1_0.csv",
		snb_dyn + "post_hasTag_tag_2_0.csv",
		snb_dyn + "post_hasTag_tag_3_0.csv",
		snb_dyn + "post_isLocatedIn_place_0_0.csv",
		snb_dyn + "post_isLocatedIn_place_1_0.csv",
		snb_dyn + "post_isLocatedIn_place_2_0.csv",
		snb_dyn + "post_isLocatedIn_place_3_0.csv",
		snb_dyn + "person_studyAt_organisation_0_0.csv",
		snb_dyn + "person_studyAt_organisation_1_0.csv",
		snb_dyn + "person_studyAt_organisation_2_0.csv",
		snb_dyn + "person_studyAt_organisation_3_0.csv",
		snb_dyn + "person_workAt_organisation_0_0.csv",
		snb_dyn + "person_workAt_organisation_1_0.csv",
		snb_dyn + "person_workAt_organisation_2_0.csv",
		snb_dyn + "person_workAt_organisation_3_0.csv"};

	spdlog::info("trying to load data from {} and {}", snb_sta, snb_dyn);
	load_snb_data(graph, node_files, rship_files);

#ifdef BUILD_INDEX
#ifdef USE_TX
	auto tx = graph->begin_transaction();
#endif
	auto idx_1 = graph->create_index("Person", "id");
	auto idx_2 = graph->create_index("Post", "id");
	auto idx_3 = graph->create_index("Comment", "id");
	auto idx_4 = graph->create_index("Place", "id");
	auto idx_5 = graph->create_index("Tag", "id");
	auto idx_6 = graph->create_index("Organisation", "id");
	auto idx_7 = graph->create_index("Forum", "id");
#ifdef USE_TX
	graph->commit_transaction();
#endif
#endif
}

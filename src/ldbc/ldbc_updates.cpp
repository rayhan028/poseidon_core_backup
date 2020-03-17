#include <iostream>
#include <boost/variant.hpp>
#include <boost/program_options.hpp>

#include "defs.hpp"
#include "graph_db.hpp"
#include "ldbc.hpp"
#include "config.h"

#include <cassert>
#include "threadsafe_queue.hpp"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

#ifdef USE_PMDK

const std::string test_path = poseidon::gPmemPath + "sf10";

struct root {
  graph_db_ptr graph;
};

#endif

double calc_avg_time(const std::vector<double>& vec) {
    double d = 0.0;
    for (auto v : vec) {
        std::cout << v << ", ";
        d += v;
    }
    std::cout << "\n";
    return d / (double)vec.size();
}

double run_query_1(graph_db_ptr gdb) {
    
    std::vector<params_tuple> parameters = {
        {(uint64_t)505, (uint64_t)61, (uint64_t)7954, (uint64_t)5, (uint64_t)9999999990999,
            "firstname1", "lastname1", "male", "1982-4-27", "2010-02-14T01:51:21.746+0000",
            "1.183.127.173", "chrome", "tamil", "ar@xyz.com",
            2001, 2012},
        {(uint64_t)990, (uint64_t)16065, (uint64_t)3179, (uint64_t)5045, (uint64_t)9999999999999,
            "firstname2", "lastname2", "male", "1997-4-27", "2010-11-14T01:51:21.746+0000",
            "13.183.167.173", "safari", "deutsch", "asASA@SSSS.com",
            1988, 1999},
        {(uint64_t)1288, (uint64_t)15835, (uint64_t)3049, (uint64_t)5753, (uint64_t)9999999900999,
            "firstname3", "lastname3", "female", "1975-12-10", "2014-01-15T01:51:21.746+0000",
            "33.13.128.183", "explorer", "english", "sss@wwww.com",
             2000, 2010},
        {(uint64_t)1288, (uint64_t)186, (uint64_t)3111, (uint64_t)6666, (uint64_t)999439900999,
            "firstname4", "lastname4", "female", "1974-12-10", "2014-01-15T01:51:21.746+0000",
            "33.13.128.183", "explorer", "english", "sss@wwww.com",
             2000, 2010},
        {(uint64_t)1350, (uint64_t)2515, (uint64_t)941, (uint64_t)2328, (uint64_t)9999999449999,
            "firstname5", "lastname5", "female", "1982-4-19", "2003-02-14T01:51:21.746+0000",
            "1.183.127.173", "chrome", "kannada", "ar@xyz.com",
             1987, 1999},
        {(uint64_t)1262, (uint64_t)40, (uint64_t)4, (uint64_t)7939, (uint64_t)888888999999,
            "firstname6", "lastname6", "female", "1997-3-27", "2005-11-14T01:51:21.746+0000",
            "13.183.167.173", "safari", "deutsch", "asASA@SSSS.com",
            2001, 2015},
        {(uint64_t)337, (uint64_t)9764, (uint64_t)3714, (uint64_t)10, (uint64_t)9989299900999,
            "firstname7", "lastname7", "female", "1975-12-10", "2008-01-15T01:51:21.746+0000",
            "33.13.128.183", "explorer", "english", "sss@wwww.com",
            1979, 1997},
        {(uint64_t)412, (uint64_t)14086, (uint64_t)6154, (uint64_t)7216, (uint64_t)912345989999,
            "firstname8", "lastname8", "female", "1996-4-17", "2009-05-14T01:51:21.746+0000",
            "10.183.117.123", "firefox", "urdu", "uuuu@ztrzrzz.com",
             1977, 1996},
        {(uint64_t)480, (uint64_t)15834, (uint64_t)4198, (uint64_t)6933, (uint64_t)7659966990999,
            "firstname9", "lastname9", "male", "2000-11-01", "2011-02-14T01:51:21.746+0000",
            "1.183.127.173", "chrome", "tamil", "ar@xyz.com",
             2005, 2016},
        {(uint64_t)526, (uint64_t)3724, (uint64_t)1327, (uint64_t)2314, (uint64_t)97999999999998,
            "firstname10", "lastname10", "male", "2003-4-27", "2007-11-14T01:51:21.746+0000",
            "13.183.167.173", "safari", "deutsch", "asASA@SSSS.com",
             2009, 2019},
        {(uint64_t)569, (uint64_t)9406, (uint64_t)4813, (uint64_t)6102, (uint64_t)8765345665433,
            "firstname11", "lastname11", "female", "2002-12-14", "2005-01-15T01:51:21.746+0000",
            "33.13.128.183", "explorer", "english", "sss@wwww.com",
            2011, 2020},
        {(uint64_t)610, (uint64_t)61, (uint64_t)1437, (uint64_t)7954, (uint64_t)90843284933333,
            "firstname12", "lastname12", "male", "2012-01-15", "2008-05-14T01:51:21.746+0000",
            "10.183.117.123", "firefox", "arabic", "uuuu@ztrzrzz.com",
            2001, 2010},
        {(uint64_t)760, (uint64_t)5479, (uint64_t)7731, (uint64_t)307, (uint64_t)9999999990999,
            "firstname13", "lastname13", "female", "2005-10-29", "2010-02-14T01:51:21.746+0000",
            "1.183.127.173", "chrome", "french", "ar@xyz.com",
             2001, 2013},
        {(uint64_t)817, (uint64_t)3751, (uint64_t)1380, (uint64_t)676, (uint64_t)6383865350979,
            "firstname14", "lastname14", "male", "1993-02-20", "2010-11-14T01:51:21.746+0000",
            "13.183.167.173", "safari", "deutsch", "asASA@SSSS.com",
             1994, 2010},
        {(uint64_t)857, (uint64_t)1409, (uint64_t)4030, (uint64_t)1211, (uint64_t)1356785432345,
            "firstname15", "lastname15", "female", "1997-12-31", "2014-01-15T01:51:21.746+0000",
            "33.13.128.183", "explorer", "english", "sss@wwww.com",
             1990, 2000},
        {(uint64_t)916, (uint64_t)6042, (uint64_t)3023, (uint64_t)4719, (uint64_t)56543245674567,
            "firstname16", "lastname16", "female", "1989-05-16", "2010-05-14T01:51:21.746+0000",
            "10.183.117.123", "firefox", "persian", "uuuu@ztrzrzz.com",
            1989, 2009},
        {(uint64_t)967, (uint64_t)389, (uint64_t)6265, (uint64_t)4344, (uint64_t)9876543234567,
            "firstname17", "lastname17", "male", "1988-05-11", "2010-02-14T01:51:21.746+0000",
            "1.183.127.173", "chrome", "tamil", "ar@xyz.com",
            1971, 1996},
        {(uint64_t)1039, (uint64_t)3540, (uint64_t)6947, (uint64_t)836, (uint64_t)8473462543452,
            "firstname18", "lastname18", "female", "1999-07-27", "2010-11-14T01:51:21.746+0000",
            "13.183.167.173", "safari", "deutsch", "asASA@SSSS.com",
             1988, 2004},
        {(uint64_t)1132, (uint64_t)3456, (uint64_t)4034, (uint64_t)5364, (uint64_t)9997654456999,
            "firstname19", "lastname19", "female", "1975-10-10", "2014-01-15T01:51:21.746+0000",
            "33.13.128.183", "explorer", "english", "sss@wwww.com",
             1987, 2007},
        {(uint64_t)1146, (uint64_t)1251, (uint64_t)379, (uint64_t)0, (uint64_t)99999989999999,
            "firstname20", "lastname20", "female", "1982-01-17", "2010-05-12T01:51:21.746+0000",
            "10.183.117.123", "firefox", "spanish", "uuuu@ztrzrzz.com",
             1980, 1997}
    };

    std::vector<double> runtimes(parameters.size());
    for (auto i = 0u; i < parameters.size(); i++) {
        result_set rs;
        assert(parameters[i].size() == 16);
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_iu_query_1(gdb, rs, parameters[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
    }
    return calc_avg_time(runtimes);
}

double run_query_2(graph_db_ptr gdb) {
    
    std::vector<params_tuple> parameters = {
        {(uint64_t)962072874067, (uint64_t)933, "2012-04-14T01:51:21.746+0000"},
        {(uint64_t)1374389534791, (uint64_t)4398046516514, "2014-02-14T01:51:21.746+0000"},
        {(uint64_t)2061584699910, (uint64_t)30786325582603, "2000-02-14T01:51:21.746+0000"},
        {(uint64_t)1511828886129, (uint64_t)24189255818939, "2012-02-14T01:51:21.746+0000"},
        {(uint64_t)687195169734, (uint64_t)30786325585585, "2016-02-14T01:51:21.746+0000"},
        {(uint64_t)1924145751643, (uint64_t)8735, "1999-02-14T01:51:21.746+0000"},
        {(uint64_t)824634129681, (uint64_t)24189255820909, "1989-02-14T01:51:21.746+0000"},
        {(uint64_t)412317270574, (uint64_t)32985348834100, "1981-02-14T01:51:21.746+0000"},
        {(uint64_t)412317270523, (uint64_t)28587302324474, "1983-02-14T01:51:21.746+0000"},
        {(uint64_t)274878318222, (uint64_t)30786325579940, "1993-02-14T01:51:21.746+0000"},
        {(uint64_t)274878318393, (uint64_t)32985348839609, "1995-02-14T01:51:21.746+0000"},
        {(uint64_t)1786706808159, (uint64_t)933, "1996-02-14T01:51:21.746+0000"},
        {(uint64_t)549756226945, (uint64_t)4398046521458, "1999-02-14T01:51:21.746+0000"},
        {(uint64_t)274878320024, (uint64_t)6597069774386, "2010-02-14T01:51:21.746+0000"},
        {(uint64_t)1236950997907, (uint64_t)28587302326516, "2010-02-14T01:51:21.746+0000"},
        {(uint64_t)549756231266, (uint64_t)4398046519071, "2014-02-14T01:51:21.746+0000"},
        {(uint64_t)687195185090, (uint64_t)1050, "2013-02-14T01:51:21.746+0000"},
        {(uint64_t)549756233724, (uint64_t)4008, "2011-02-14T01:51:21.746+0000"},
        {(uint64_t)1374389955160, (uint64_t)2199023260990, "2013-02-14T01:51:21.746+0000"},
        {(uint64_t)1236951003301, (uint64_t)4398046521879, "2000-02-14T01:51:21.746+0000"}
    };

    std::vector<double> runtimes(parameters.size());
    for (auto i = 0u; i < parameters.size(); i++) {
        result_set rs;
        assert(parameters[i].size() == 3);
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_iu_query_2(gdb, rs, parameters[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
    }
    //std::cout << rs;
    return calc_avg_time(runtimes);
}

double run_query_3(graph_db_ptr gdb) {
    
    std::vector<params_tuple> parameters = {
        {(uint64_t)1236950871584, (uint64_t)8796093026886, "2012-04-14T01:51:21.746+0000"},
        {(uint64_t)2061584593803, (uint64_t)10995116283227, "2012-04-14T01:51:21.746+0000"},
        {(uint64_t)1786706688521, (uint64_t)19791209308983, "2012-04-14T01:51:21.746+0000"},
        {(uint64_t)1649267736072, (uint64_t)13194139540404, "2012-04-14T01:51:21.746+0000"},
        {(uint64_t)1649267736843, (uint64_t)512, "2012-04-14T01:51:21.746+0000"},
        {(uint64_t)2061584599603, (uint64_t)35184372093792, "2012-04-14T01:51:21.746+0000"},
        {(uint64_t)1099511928595, (uint64_t)15393162795008, "2012-04-14T01:51:21.746+0000"},
        {(uint64_t)412317165378, (uint64_t)3325, "2012-04-14T01:51:21.746+0000"},
        {(uint64_t)2061584610800, (uint64_t)933, "2012-04-14T01:51:21.746+0000"},
        {(uint64_t)1649267752192, (uint64_t)19791209310439, "2012-04-14T01:51:21.746+0000"},
        {(uint64_t)1649267753602, (uint64_t)30786325584319, "2012-04-14T01:51:21.746+0000"},
        {(uint64_t)824634033764, (uint64_t)6597069777216, "2012-04-14T01:51:21.746+0000"},
        {(uint64_t)962072988978, (uint64_t)32985348841418, "2012-04-14T01:51:21.746+0000"},
        {(uint64_t)824634036612, (uint64_t)21990232564124, "2012-04-14T01:51:21.746+0000"},
        {(uint64_t)1374389852068, (uint64_t)4398046514155, "2012-04-14T01:51:21.746+0000"},
        {(uint64_t)1786706713284, (uint64_t)13194139535993, "2012-04-14T01:51:21.746+0000"},
        {(uint64_t)1236950900888, (uint64_t)28587302323441, "2012-04-14T01:51:21.746+0000"},
        {(uint64_t)1511828809498, (uint64_t)8796093030808, "2012-04-14T01:51:21.746+0000"},
        {(uint64_t)1924145672762, (uint64_t)32985348843050, "2012-04-14T01:51:21.746+0000"},
        {(uint64_t)1374389860075, (uint64_t)9616, "2012-04-14T01:51:21.746+0000"}
    };

    std::vector<double> runtimes(parameters.size());
    for (auto i = 0u; i < parameters.size(); i++) {
        result_set rs;
        assert(parameters[i].size() == 3);
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_iu_query_3(gdb, rs, parameters[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
    }
    return calc_avg_time(runtimes);
}

double run_query_4(graph_db_ptr gdb) {
    
    std::vector<params_tuple> parameters = {
        {(uint64_t)8796093026886, (uint64_t)7164, (uint64_t)9999999999,
        "Album 112 of Peter Jones", "2015-12-21T12:50:35.556+0100"},
        {(uint64_t)4001, (uint64_t)6, (uint64_t)9998888999,
        "Album 2222 of abcde", "2015-10-29T12:50:35.556+0660"},
        {(uint64_t)24189255816341, (uint64_t)12992, (uint64_t)9990009999,
        "Album 22 of fefdsfasdas", "2015-011-15T12:50:35.556+0160"},
        {(uint64_t)32985348837988, (uint64_t)15672, (uint64_t)9993339999,
        "Album 66 of john", "2015-01-24T12:50:66.556+0100"},
        {(uint64_t)4398046512636, (uint64_t)1895, (uint64_t)99334569999,
        "Album 55 of willaims", "2015-06-124T12:50:66.556+0100"},
        {(uint64_t)8796093032603, (uint64_t)9769, (uint64_t)9677799999,
        "Album 75 of Elizibeth", "2015-06-24T12:50:88.556+0100"},
        {(uint64_t)8796093029724, (uint64_t)12700, (uint64_t)87654567999,
        "Album 75 of Samuel", "2015-06-24T12:45:35.556+0100"},
        {(uint64_t)10995116287090, (uint64_t)16079, (uint64_t)99967654999,
        "Album 89 of Micheal", "2015-05-24T12:50:35.556+0100"},
        {(uint64_t)2199023256684, (uint64_t)0, (uint64_t)1111111111,
        "Album 109 of Mary", "2007-06-22T12:10:35.556+0100"},
        {(uint64_t)32985348840140, (uint64_t)14348, (uint64_t)2345676543,
        "Album 99 of Silva", "2017-12-24T12:50:35.666+0100"},
        {(uint64_t)28587302330205, (uint64_t)16028, (uint64_t)8746364784,
        "Album 12 of Paul john adams", "2001-11-17T12:50:35.576+0100"},
        {(uint64_t)32985348844015, (uint64_t)6797, (uint64_t)5674337488,
        "Album 33 of Paul", "2016-10-24T12:50:35.556+0600"},
        {(uint64_t)19791209300787, (uint64_t)14624, (uint64_t)7583536876,
        "Album 1 of Albert", "1998-06-20T12:50:35.56+0100"},
        {(uint64_t)19791209309916, (uint64_t)8615, (uint64_t)57447677474,
        "Album 33 of samuel", "1999-07-19T12:60:65.556+0100"},
        {(uint64_t)19791209306932, (uint64_t)4500, (uint64_t)637475895858,
        "Album 12 of Alejandro Araya", "2000-05-24T12:50:35.566+0100"},
        {(uint64_t)35184372093792, (uint64_t)2964, (uint64_t)748473647636,
        "Album 78371 of xyz", "2001-06-24T12:50:65.556+0160"},
        {(uint64_t)1129, (uint64_t)1014, (uint64_t)84736345757,
        "Album 1 of Peter Jones", "2011-06-24T12:50:35.556+0100"},
        {(uint64_t)24189255818615, (uint64_t)12182, (uint64_t)73564747477,
        "Album 56 of Peter Jones", "2012-05-22T12:50:35.556+0100"},
        {(uint64_t)10995116282209, (uint64_t)14693, (uint64_t)477574364574,
        "Album 45 of Peter Jones", "2013-03-21T12:50:65.556+0100"},
        {(uint64_t)2199023265882, (uint64_t)15987, (uint64_t)9991100009,
        "Album 111 of Peter Jones", "2013-02-20T12:50:35.666+0100"}
    };

    std::vector<double> runtimes(parameters.size());
    for (auto i = 0u; i < parameters.size(); i++) {
        result_set rs;
        assert(parameters[i].size() == 5);
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_iu_query_4(gdb, rs, parameters[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
    }
    return calc_avg_time(runtimes);
}

double run_query_5(graph_db_ptr gdb) {
    
    std::vector<params_tuple> parameters = {
        {(uint64_t)4194, (uint64_t)1786706395137, "2011-01-02T06:43:51.955+0000"},
        {(uint64_t)6597069777454, (uint64_t)1099511698394, "2012-03-24T16:37:35.817+0000"},
        {(uint64_t)28587302324814, (uint64_t)1236950666805, "2012-01-17T20:19:22.276+0000"},
        {(uint64_t)28587302326940, (uint64_t)1236950681265, "2012-01-14T11:14:07.725+0000"},
        {(uint64_t)10995116283196, (uint64_t)1786706430791, "2011-01-02T06:43:51.955+0000"},
        {(uint64_t)2199023258871, (uint64_t)1649267452668, "2011-10-24T06:18:38.139+0000"},
        {(uint64_t)6597069766938, (uint64_t)962072719206, "2012-04-09T16:44:42.685+0000"},
        {(uint64_t)6597069773271, (uint64_t)687194836136, "2011-01-02T06:43:51.955+0000"},
        {(uint64_t)32985348838262, (uint64_t)1924145436065, "2011-01-02T06:43:51.955+0000"},
        {(uint64_t)13194139543446, (uint64_t)1236951634156, "2012-06-13T03:32:42.855+0000"},
        {(uint64_t)32985348843050, (uint64_t)1374389560523, "2011-01-02T06:43:51.955+0000"},
        {(uint64_t)30786325588321, (uint64_t)1924145358674, "2011-01-02T06:43:51.955+0000"},
        {(uint64_t)24189255812103, (uint64_t)2061584348659, "2011-01-02T06:43:51.955+0000"},
        {(uint64_t)28587302323229, (uint64_t)824633807283, "2011-01-02T06:43:51.955+0000"},
        {(uint64_t)2199023255685, (uint64_t)1099511714359, "2011-01-02T06:43:51.955+0000"},
        {(uint64_t)8824, (uint64_t)962072697006, "2011-01-02T06:43:51.955+0000"},
        {(uint64_t)7763, (uint64_t)1786706442735, "2011-01-02T06:43:51.955+0000"},
        {(uint64_t)8796093024822, (uint64_t)1374389623873, "2012-06-13T03:32:42.855+0000"},
        {(uint64_t)10995116285557, (uint64_t)1511829542447, "2011-01-02T06:43:51.955+0000"},
        {(uint64_t)933, (uint64_t)2061585359993, "2011-07-16T18:37:42.578+0000"}
    };

    std::vector<double> runtimes(parameters.size());
    for (auto i = 0u; i < parameters.size(); i++) {
        result_set rs;
        assert(parameters[i].size() == 3);
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_iu_query_5(gdb, rs, parameters[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
    }
    return calc_avg_time(runtimes);
}

double run_query_6(graph_db_ptr gdb) {
    
    std::vector<params_tuple> parameters = {
        {(uint64_t)10027, (uint64_t)2199024313492, (uint64_t)50,
        (uint64_t)1679, (uint64_t)999634181970,
        "", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "english",
        "About Alexander I of Russia",
        123},
        {(uint64_t)30786325585301, (uint64_t)2061584333411, (uint64_t)1434,
        (uint64_t)7513, (uint64_t)9996543581970,
        "", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "Spanish", "About Adolf Hitler, tler's "
        "political views. His writings and meAbout John Milton, eved international renown within his lifetiAbout "
        "Robert Fripp, , a technique often associated with the banAbout Marilyn Monroe",
        424},
        {(uint64_t)10995116279657, (uint64_t)1511828582893, (uint64_t)220,
        (uint64_t)11072, (uint64_t)999657774570,
        "", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "german", "bout Aurangzeb, ries in the south "
        "expanded the MAbout Peter Hain, ed to his resignation in 2008. HAbout Clement Attlee, came Prime Minister "
        "in 1979. HisAbout Francis Ford Coppola, sese, Terrence Malick, Robert AlAbout Fai|",
        443},
        {(uint64_t)17592186052886, (uint64_t)412316920493, (uint64_t)1015,
        (uint64_t)14805, (uint64_t)999664566470,
        "", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "Urdu", "About Jim Carrey, ic, slapstick "
        "performaAbout Ludacris,  (2004), Release TheraAbout ",
        434},
        {(uint64_t)4398046515029, (uint64_t)824633820143, (uint64_t)1315,
        (uint64_t)6329, (uint64_t)9996561970,
        "", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "tamil", "About Evo Morales, ma "
        "(born October 26, 1959), popularly known as Evo, is a Bolivian politician and activist, serving as "
        "the 80th President of Bolivia, a position that he has held since 2006. He is also the ",
        98},
        {(uint64_t)32985348834655, (uint64_t)962072743523, (uint64_t)273,
        (uint64_t)0, (uint64_t)9966664181970,
        "", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "hindi", "About abc def ghi jkl mno pqrs "
        "tuv wxyz ABC DEF GHI JKL MNO PQRS TUV WXYZ !About abc def ghi jkl mno pqrs tuv wxyz ABC DEF GHI JKL MNO "
        "PQRS TUV WXYZ ,About abc def ghi jkl mno pqrs tuv wxyz ABC DEF GHI JKL MNO PQRS TUV WXYZ About abc def "
        "ghi jkl mno pqrs tuv wxyz ABC DEF GHI JKL MNO PQRS TUV WXYZ About abc def ghi jkl mno pqrs tuv wxyz ABC "
        "DEF GHI JKL MNO PQRS TUV WXYZ About abc def ghi jkl mno pqrs tuv wxyz ABC DEF GHI JKL MNO PQRS TUV WXYZ "
        "About abc def ghi jkl mno pqrs tuv wxyz ABC DEF GHI JKL MNO PQRS TUV WXYZ  ",
        777},
        {(uint64_t)2199023256530, (uint64_t)1099512681152, (uint64_t)991,
        (uint64_t)16079, (uint64_t)994396660,
        "", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "korean", "About Evo Morales, on measures. "
        "Born into a working class Aymara family in Isallawi, Orinoca Canton, Evo grew up aiding hi",
        234},
        {(uint64_t)4398046515173, (uint64_t)412316908754, (uint64_t)1267,
        (uint64_t)6560, (uint64_t)9994578970,
        "", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "german", "About Lachlan Macquarie, s, "
        "Australia from 1810 to 1821 and had a leading role in the social, economic and architectural "
        "development of the colony. He is c",
        789},
        {(uint64_t)24189255817322, (uint64_t)274877930860, (uint64_t)585,
        (uint64_t)1100, (uint64_t)999637777970,
        "", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "english", "About Karl Marx,  where he "
        "became interested in the philosophical ideas of the Young Hegelians. In 1836, he became engaged to "
        "Jenny von Westphalen, marrying her in 1843. After his studies, he wrote for a radical newspaper in "
        "Cologne, and began to w",
        907},
        {(uint64_t)30786325587134, (uint64_t)2061584395107, (uint64_t)1122,
        (uint64_t)13489, (uint64_t)999633666970,
        "", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "arabic", "About Karl Marx, for such goods. "
        "Heavily critical of the current socio-economic form of society, capitalism, he called it the dictatorship "
        "of the bourgeoisie, believing it t",
        445},
        {(uint64_t)32985348842038, (uint64_t)47780, (uint64_t)1459,
        (uint64_t)2214, (uint64_t)999634181970,
        "", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "arabic", "About Brian Mulroney, es Tax, and "
        "the rejection of constitutional reforms such as the ids tht is the again",
        23},
        {(uint64_t)26388279076840, (uint64_t)1099511715698, (uint64_t)0,
        (uint64_t)5836, (uint64_t)999466181970,
        "", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "english", "About Ivan Ljubičić, s. Ljubičić "
        "used the Head Youtek Extreme Pro Racquet, after using the Babolat Pure DrAbout Dangerous and Moving, "
        "released on October 5, 2005 in Japan",
        434},
        {(uint64_t)8796093024736, (uint64_t)1786706434469, (uint64_t)919,
        (uint64_t)15329, (uint64_t)976584181970,
        "", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "german", "red Croatian tennis player. "
        "His career-high ATP ranking was no. 3. Tall and powerfully built, he was notedAbout The Rubberband Man,  "
        "100, and topped the U.S. R&B chart at the end of 1976. It",
        244},
        {(uint64_t)21990232556886, (uint64_t)2061584390250, (uint64_t)180,
        (uint64_t)5081, (uint64_t)999687881970,
        "", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "french", "About Arnold Schwarzenegger, "
        "Governor and Terminator). As a Republican, he was first elected on October 7, 2003, in a special recall "
        "election to replace then-Governor Gray Davis. Schwarzenegger was sworn in",
        45},
        {(uint64_t)26388279068827, (uint64_t)962072688578, (uint64_t)1192,
        (uint64_t)971, (uint64_t)999634181970,
        "", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "german", "About Sammy Sosa, Sosa has long been "
        "thAbout William Penn, f William Penn, foundAbout Magical Mystery Tour,  of the record",
        55},
        {(uint64_t)6597069774626, (uint64_t)16, (uint64_t)232,
        (uint64_t)10, (uint64_t)9996341888870,
        "", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "malayalam", "About Alexander Downer, s also the "
        "Leader of the Opposition for eight months frAbout Bourbon Restoration, eon (1804–1814/1815) – when a "
        "coalition of European powAbo",
        987},
        {(uint64_t)15393162793237, (uint64_t)1099511682067, (uint64_t)767,
        (uint64_t)7959, (uint64_t)999675474770,
        "", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "kannada", "About Napoleon, continuation of "
        "the wars sparked by the French Revolution of 1789, they revolAbout Jamie Foxx, n December 13, 1967), "
        "professionally know",
        12},
        {(uint64_t)6597069777240, (uint64_t)1786706434469, (uint64_t)1216,
        (uint64_t)13491, (uint64_t)9123456970,
        "", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "french", "About Augustine of Hippo,  "
        "theology, accommodating a variety of methods and different pers",
        24},
        {(uint64_t)30786325580042, (uint64_t)549756869111, (uint64_t)1417,
        (uint64_t)6361, (uint64_t)9996341234986,
        "", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "telugu", "Francis of Assisi, went to "
        "Egypt in anAbout John Stuart Mill, onception of libertAbout Robert Fripp, p",
        443},
        {(uint64_t)13194139535252, (uint64_t)0, (uint64_t)634,
        (uint64_t)14277, (uint64_t)6576575745,
        "", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "japanese", "About Camille Saint-Saëns, "
        "16 December 1921) was About David Hume, lly on utilitariani",
        989}
    };

    std::vector<double> runtimes(parameters.size());
    for (auto i = 0u; i < parameters.size(); i++) {
        result_set rs;
        assert(parameters[i].size() == 12);
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_iu_query_6(gdb, rs, parameters[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
    }
    return calc_avg_time(runtimes);
}

double run_query_7(graph_db_ptr gdb) {
    
    std::vector<params_tuple> parameters = {
        {(uint64_t)19791209305763, (uint64_t)1786707069811, (uint64_t)1459,
        (uint64_t)1426, (uint64_t)10997654432472,
        "2012-01-09T11:49:15.991+0000", "91.145.169.27", "safari", "About Chen Shui-bian, e Legislative "
        "YuanAbout Éamon de Valera, Pat Coogan sees hiAbout",
        54},
        {(uint64_t)24189255819940, (uint64_t)412316984810, (uint64_t)805,
        (uint64_t)9, (uint64_t)12343452472,
        "2012-07-20T04:51:23.927+0000", "204.79.193.83", "explorer", "About Love to Love You Baby, irst "
        "to be released internationally and in the United Sta",
        78},
        {(uint64_t)24189255818806, (uint64_t)1649267467066, (uint64_t)1235,
        (uint64_t)8808, (uint64_t)765456776543,
        "2011-10-07T04:46:03.896+0000", "91.14.169.237", "safari", "About Ashoka, oka, his legend is "
        "relateAbout Theodore Roosevelt, he Russo-Japanese War, foAbout Ma",
        75},
        {(uint64_t)15393162791859, (uint64_t)1374390139472, (uint64_t)20,
        (uint64_t)7237, (uint64_t)2893289859202,
        "2012-01-09T11:49:15.991+0000", "91.159.169.22", "chrome", "About Edgar Allan Poe, is father "
        "abandoned the family. Poe was tAbout Khanate of",
        543},
        {(uint64_t)19791209302403, (uint64_t)412319660593, (uint64_t)796,
        (uint64_t)13525, (uint64_t)95572870933,
        "2010-11-02T01:06:54.399+0000", "91.1467.169.207", "safari", "About Elena Likhovtseva, "
        "rter-finals at the Sony Ericsson Open in MiaAbout Please Come|",
        343},
        {(uint64_t)13194139538552, (uint64_t)1236950581248, (uint64_t)315,
        (uint64_t)7380, (uint64_t)95957502,
        "2012-01-09T11:49:15.991+0000", "91.33.169.255", "firefox", "About Charles V, Holy "
        "Roman Emperor, the New WorlAbout Freddie Mercury, ock star. InAbout Al Gore, 0. Victory iAbout",
        99},
        {(uint64_t)8796093026304, (uint64_t)1786708729389, (uint64_t)53,
        (uint64_t)7534, (uint64_t)95959342,
        "2012-01-09T11:49:15.991+0000", "91.149.11.27", "safari", "About Imelda Marcos, ected as "
        "membAbout Dead Leaves and the Dirty Ground, e UK Albums",
        31},
        {(uint64_t)26388279077346, (uint64_t)962073135697, (uint64_t)959,
        (uint64_t)2493, (uint64_t)6602232,
        "2012-01-09T11:49:15.991+0000", "91.149.169.2237", "safari", "About John Howard, died in "
        "combaAbout Isabella I of Castile, r right to thAbo",
        89},
        {(uint64_t)4398046517791, (uint64_t)2176481, (uint64_t)1309,
        (uint64_t)13150, (uint64_t)8765432456,
        "2012-01-09T11:49:15.991+0000", "91.149.169.27", "netscape", "About Imelda Marcos, tatives "
        "to represent Ilocos Norte's seAbout Sultanate",
        90},
        {(uint64_t)6597069773790, (uint64_t)2061586938095, (uint64_t)1434,
        (uint64_t)6452, (uint64_t)4009491401,
        "2012-01-09T11:49:15.991+0000", "204.79.194.82", "safari", "About Germany, hird largest "
        "importer of goods. The country has developedAbout Kingdom of Kandy, kept European col",
        353},
        {(uint64_t)345, (uint64_t)2199024637094, (uint64_t)585,
        (uint64_t)1808, (uint64_t)929848909740,
        "2012-01-09T11:49:15.991+0000", "202.719.194.222", "firefox", "Internet Explorer|About "
        "Jorge Luis Borges, by the LatinAbout Arnold Schoenberg, uermann, and About",
        233},
        {(uint64_t)8796093029498, (uint64_t)3, (uint64_t)180,
        (uint64_t)10432, (uint64_t)10432,
        "2012-01-09T11:49:15.991+0000", "41.149.119.247", "safari", "About Georg Wilhelm "
        "Friedrich Hegel, that mind or spirit manifested itself in Ab",
        89},
        {(uint64_t)28587302329250, (uint64_t)412323272449, (uint64_t)1241,
        (uint64_t)14790, (uint64_t)75891334,
        "2012-01-09T11:49:15.991+0000", "41.149.169.27", "safari", "About Hey Sexy Lady, "
        "the title of a song recorded by Jamacian-American reggae artist ShaggAbout Brown "
        "Album, nder; as such, this is their",
        67},
        {(uint64_t)8796093028204, (uint64_t)549758755972, (uint64_t)1111,
        (uint64_t)4939, (uint64_t)589254324982,
        "2012-01-09T11:49:15.991+0000", "91.149.169.27", "mozilla", "About George Frideric "
        "Handel, s oratorios is an ethical one. They aAbout Jay-Z, ey Carter (born December "
        "4, 1969), beAbout Garth Brooks, ted int",
        67},
        {(uint64_t)26388279072962, (uint64_t)1511828865755, (uint64_t)605,
        (uint64_t)1236, (uint64_t)9570000042325,
        "2012-01-09T11:49:15.991+0000", "88.19.169.217", "safari", "About Bruce Lee,  "
        "Hong Kong martial arts filmAbout Greece, irthplace of democracy",
        33},
        {(uint64_t)94, (uint64_t)1924145860444, (uint64_t)30,
        (uint64_t)8341, (uint64_t)8505838595953,
        "2012-01-09T11:49:15.991+0000", "294.34.294.10", "safari", "About Nero, t. "
        "The study of NeroAbout Eleanor Rigby, ing quartet arrangemAbout New Zealand,  the New Zealand ArmAbou",
        123},
        {(uint64_t)19791209303234, (uint64_t)1374390532480, (uint64_t)215,
        (uint64_t)12325, (uint64_t)574394839,
        "2011-10-08T14:00:21.609+0000", "91.149.129.237", "safari", "bout Amy Winehouse,  Love Is a Losing "
        "Game. Winehouse died of alcAbout Where Is the Love",
        689},
        {(uint64_t)8796093025123, (uint64_t)824635868714, (uint64_t)1130,
        (uint64_t)7066, (uint64_t)987503453,
        "2012-05-13T13:41:59.023+0000", "234.22.194.42", "chrome", "About William IV of the United "
        "Kingdom, ribbean, but saw little actual fighting. SA",
        77},
        {(uint64_t)6597069773503, (uint64_t)1924149676560, (uint64_t)449,
        (uint64_t)11251, (uint64_t)86099534,
        "2011-10-18T10:27:27.756+0000", "91.149.169.27", "safari", "About Carl Jung, while still "
        "mAbout Katy Perry, five number oAbout Brian W",
        12},
        {(uint64_t)21990232559210, (uint64_t)1786712995900, (uint64_t)50,
        (uint64_t)0, (uint64_t)100918497450,
        "2011-06-29T00:05:26.944+0000", "266.79.14.02", "Opera", "|About Marcelo Melo, he "
        "seventh BrazilianAbout Pope Pius XI, e Mystici Corpori",
        79}
    };

    std::vector<double> runtimes(parameters.size());
    for (auto i = 0u; i < parameters.size(); i++) {
        result_set rs;
        assert(parameters[i].size() == 10);
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_iu_query_7(gdb, rs, parameters[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
    }
    return calc_avg_time(runtimes);
}

double run_query_8(graph_db_ptr gdb) {
    
    std::vector<params_tuple> parameters = {
        {(uint64_t)555, (uint64_t)24189255811086, "2011-11-21T02:27:05.636+0000"},
        {(uint64_t)15393162793955, (uint64_t)24189255814147, "2010-03-12T17:28:43.563+0000"},
        {(uint64_t)4398046522002, (uint64_t)21990232556891, "2011-12-10T17:41:10.929+0000"},
        {(uint64_t)35184372093792, (uint64_t)933, "1999-02-26T21:37:35.528+0000"},
        {(uint64_t)9005, (uint64_t)8796093030323, "2012-07-20T16:42:08.327+0000"},
        {(uint64_t)4398046521818, (uint64_t)8649, "2010-12-09T18:13:19.867+000"},
        {(uint64_t)15393162794170, (uint64_t)15393162796413, "2011-05-03T18:56:54.450+0000"},
        {(uint64_t)15393162797671, (uint64_t)28587302324497, "2011-03-19T05:23:21.691+0000"},
        {(uint64_t)24189255819297, (uint64_t)8796093030039, "2011-01-30T00:44:45.595+0000"},
        {(uint64_t)28587302328644, (uint64_t)30786325581382, "2010-12-15T23:27:00.248+0000"},
        {(uint64_t)30786325588071, (uint64_t)30786325578904, "2010-04-13T16:23:31.692+0000"},
        {(uint64_t)10027, (uint64_t)35184372097876, "2010-07-15T04:56:46.299+0000"},
        {(uint64_t)17592186044551, (uint64_t)13194139535717, "2012-07-11T03:57:44.959+0000"},
        {(uint64_t)933, (uint64_t)2199023261114, "2012-02-01T03:30:59.228+0001"},
        {(uint64_t)32985348835435, (uint64_t)2199023266429, "2010-07-16T03:52:16.375+0000"},
        {(uint64_t)32985348838898, (uint64_t)24189255819114, "2010-06-08T07:34:47.276+0000"},
        {(uint64_t)28587302322743, (uint64_t)6478, "2002-03-03T19:22:42.796+0000"},
        {(uint64_t)2862, (uint64_t)30786325580396, "2011-12-05T07:04:55.336+00000"},
        {(uint64_t)17592186051428, (uint64_t)15393162799139, "2010-07-08T13:44:13.873+0000"},
        {(uint64_t)32985348838643, (uint64_t)4398046514193, "2019-06-03T06:17:49.321+0000"}
    };

    std::vector<double> runtimes(parameters.size());
    for (auto i = 0u; i < parameters.size(); i++) {
        result_set rs;
        assert(parameters[i].size() == 3);
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_iu_query_8(gdb, rs, parameters[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
    }
    return calc_avg_time(runtimes);
}

void run_benchmark(graph_db_ptr gdb) {
    double t = 0.0;
    t = run_query_1(gdb);
    spdlog::info("Query #1: {} msecs", t);
    t = run_query_2(gdb);
    spdlog::info("Query #2: {} msecs", t);
    t = run_query_3(gdb);
    spdlog::info("Query #3: {} msecs", t);
    t = run_query_4(gdb);
    spdlog::info("Query #4: {} msecs", t);
    t = run_query_5(gdb);
    spdlog::info("Query #5: {} msecs", t);
    t = run_query_6(gdb);
    spdlog::info("Query #6: {} msecs", t);
    t = run_query_7(gdb);
    spdlog::info("Query #7: {} msecs", t);
    t = run_query_8(gdb);
    spdlog::info("Query #8: {} msecs", t);
}

/* ---------------------------------------------------------------------------- */

using namespace boost::program_options;

int main(int argc, char **argv) {
  std::string db_name;

 try {
    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "Help")
        ("verbose,v", bool_switch()->default_value(false), "Verbose - show debug output")
        ("db,d", value<std::string>(&db_name)->required(),"Database name (required)");

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
      std::cout << "Poseidon Graph Database LDBC Benchmark Version " << POSEIDON_VERSION
                << "\n"
                << desc << '\n';
      return -1;
    }

    notify(vm);

      } catch (const error &ex) {
    std::cerr << ex.what() << '\n';
    return -1;
  }

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
  auto &graph = q->graph;
  graph->runtime_initialize();
#else
  auto graph = p_make_ptr<graph_db>(db_name);
#endif

  node::id_t first_insert_node = graph->get_relationships()->as_vec().first_available();
  relationship::id_t first_insert_rship = graph->get_relationships()->as_vec().first_available();

  run_benchmark(graph);
#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif
  //delete all created nodes and relationships
  node::id_t last_insert_node = graph->get_relationships()->as_vec().first_available();
  relationship::id_t last_insert_rship = graph->get_relationships()->as_vec().first_available();
  for (node::id_t i = first_insert_node; i < last_insert_node; i++)
    graph->delete_node(i);
  for (relationship::id_t i = first_insert_rship; i < last_insert_rship; i++)
    graph->delete_relationship(i);

  // assert all created nodes and relationships have been deleted
  node::id_t next_insert_node = graph->get_relationships()->as_vec().first_available();
  relationship::id_t next_insert_rship = graph->get_relationships()->as_vec().first_available();
  //assert(first_insert_node == next_insert_node);
#ifdef USE_TX
  graph->commit_transaction();
#endif
}

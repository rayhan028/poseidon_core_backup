#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do
                          // this in one cpp file

#include <boost/algorithm/string.hpp>

#include "catch.hpp"
#include "config.h"
#include "defs.hpp"
#include "ldbc.hpp"
#include "query.hpp"

#ifdef USE_PMDK
namespace nvm = pmem::obj;

#define PMEMOBJ_POOL_SIZE ((unsigned long long)(1024 * 1024 * 40000ull)) 

const std::string test_path = poseidon::gPmemPath + "ldbc_test";

nvm::pool_base prepare_pool() {
  auto pop = nvm::pool_base::create(test_path, "", PMEMOBJ_POOL_SIZE);
  return pop;
}
#endif


/**--------------------------------------------------------------------------------
 * ------------------------- GRAPH DATA -------------------------------------------
 * --------------------------------------------------------------------------------
 * --------------------------------------------------------------------------------
 * */

graph_db_ptr create_graph1(
#ifdef USE_PMDK
    nvm::pool_base &pop
#endif
) {
#ifdef USE_PMDK
  graph_db_ptr graph;
  nvm::transaction::run(pop, [&] { graph = p_make_ptr<graph_db>(); });
#else
  auto graph = p_make_ptr<graph_db>();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  // TODO: create LDBC SNB data
    auto mahinda = graph->add_node(
      // id|firstName|lastName|gender|birthday|creationDate|locationIP|browserUsed
      // 933|Mahinda|Perera|male|1989-12-03|2010-02-14T15:32:10.447+0000|119.235.7.103|Firefox
      "Person",
      {{"id", boost::any(933)},
       {"firstName", boost::any(std::string("Mahinda"))},
       {"lastName", boost::any(std::string("Perera"))},
       {"gender", boost::any(std::string("male"))},
       {"birthday", boost::any(builtin::datestring_to_int("1989-12-03"))},
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2010-02-14 15:32:10.447"))},
       {"locationIP", boost::any(std::string("119.235.7.103"))},
       {"browser", boost::any(std::string("Firefox"))}});
  auto meera = graph->add_node(
      // 10027|Meera|Rao|female|1982-12-08|2010-01-22T19:59:59.221+0000|49.249.98.96|Firefox
      "Person",
      {{"id", boost::any(10027)},
       {"firstName", boost::any(std::string("Meera"))},
       {"lastName", boost::any(std::string("Rao"))},
       {"gender", boost::any(std::string("female"))},
       {"birthday", boost::any(builtin::datestring_to_int("1982-12-08"))},
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2010-01-22 19:59:59.221"))},
       {"locationIP", boost::any(std::string("49.249.98.96"))},
       {"browser", boost::any(std::string("Firefox"))}});
  auto baruch = graph->add_node(
      // 4139|Baruch|Dego|male|1987-10-25|2010-01-28T01:38:17.824+0000|213.55.127.9|Internet Explorer
      "Person",
      {{"id", boost::any(4139)},
       {"firstName", boost::any(std::string("Baruch"))},
       {"lastName", boost::any(std::string("Dego"))},
       {"gender", boost::any(std::string("male"))},
       {"birthday", boost::any(builtin::datestring_to_int("1987-10-25"))},
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2010-01-28 01:38:17.824"))},
       {"locationIP", boost::any(std::string("213.55.127.9"))},
       {"browser", boost::any(std::string("Internet Explorer"))}}); 
  auto fritz = graph->add_node(
      // 6597069777240|Fritz|Muller|female|1987-12-01|2010-08-24T20:13:46.569+0000|46.19.159.176|Safari
      "Person",
      {{"id", boost::any(65970697)},
       {"firstName", boost::any(std::string("Fritz"))},
       {"lastName", boost::any(std::string("Muller"))},
       {"gender", boost::any(std::string("female"))},
       {"birthday", boost::any(builtin::datestring_to_int("1987-12-01"))},
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2010-08-24 20:13:46.569"))},
       {"locationIP", boost::any(std::string("46.19.159.176"))},
       {"browser", boost::any(std::string("Safari"))}});
  auto andrei = graph->add_node(
      // 10995116284808|Andrei|Condariuc|male|1982-02-04|2010-12-26T14:40:36.649+0000|92.39.58.88|Chrome
      "Person",
      {{"id", boost::any(10995116)},
       {"firstName", boost::any(std::string("Andrei"))},
       {"lastName", boost::any(std::string("Condariuc"))},
       {"gender", boost::any(std::string("male"))},
       {"birthday", boost::any(builtin::datestring_to_int("1982-02-04"))},
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2010-12-26 14:40:36.649"))},
       {"locationIP", boost::any(std::string("92.39.58.88"))},
       {"browser", boost::any(std::string("Chrome"))}});
  auto ottoR = graph->add_node(
      // 32985348838375|Otto|Richter|male|1980-11-22|2012-07-12T03:11:27.663+0000|204.79.128.176|Firefox
      "Person",
      {{"id", boost::any(838375)},
       {"firstName", boost::any(std::string("Otto"))},
       {"lastName", boost::any(std::string("Richter"))},
       {"gender", boost::any(std::string("male"))},
       {"birthday", boost::any(builtin::datestring_to_int("1980-11-22"))},
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2012-07-12 03:11:27.663"))},
       {"locationIP", boost::any(std::string("204.79.128.176"))},
       {"browser", boost::any(std::string("Firefox"))}});
  auto ottoB = graph->add_node(
      // 32985348833579|Otto|Becker|male|1989-09-23|2012-09-03T07:26:57.953+0000|31.211.182.228|Safari
      "Person",
      {{"id", boost::any(833579)},
       {"firstName", boost::any(std::string("Otto"))},
       {"lastName", boost::any(std::string("Becker"))},
       {"gender", boost::any(std::string("male"))},
       {"birthday", boost::any(builtin::datestring_to_int("1989-09-23"))},
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2012-09-03 07:26:57.953"))},
       {"locationIP", boost::any(std::string("31.211.182.228"))},
       {"browser", boost::any(std::string("Safari"))}});
  auto hoChi = graph->add_node(
      // 4194|Hồ Chí|Do|male|1988-10-14|2010-02-15T00:46:17.657+0000|103.2.223.188|Internet Explorer
      "Person",
      {{"id", boost::any(4194)},
       {"firstName", boost::any(std::string("Hồ Chí"))}, 
       {"lastName", boost::any(std::string("Do"))},
       {"gender", boost::any(std::string("male"))},
       {"birthday", boost::any(builtin::datestring_to_int("1988-10-14"))},
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2010-02-15 00:46:17.657"))},
       {"locationIP", boost::any(std::string("103.2.223.188"))},
       {"browser", boost::any(std::string("Internet Explorer"))}}); 
  auto lomana = graph->add_node(
      // 15393162795439|Lomana Trésor|Kanam|male|1986-09-22|2011-04-02T23:53:29.932+0000|41.76.137.230|Chrome
      "Person",
      {{"id", boost::any(15393)},
       {"firstName", boost::any(std::string("Lomana Trésor"))},
       {"lastName", boost::any(std::string("Kanam"))},
       {"gender", boost::any(std::string("male"))},
       {"birthday", boost::any(builtin::datestring_to_int("1986-09-22"))},
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2011-04-02 23:53:29.932"))},
       {"locationIP", boost::any(std::string("41.76.137.230"))},
       {"browser", boost::any(std::string("Chrome"))}});
  auto amin = graph->add_node(
      // 19791209307382|Amin|Kamkar|male|1989-05-24|2011-08-30T05:41:09.519+0000|81.28.60.168|Internet Explorer
      "Person",
      {{"id", boost::any(19791)},
       {"firstName", boost::any(std::string("Amin"))},
       {"lastName", boost::any(std::string("Kamkar"))},
       {"gender", boost::any(std::string("male"))},
       {"birthday", boost::any(builtin::datestring_to_int("1989-05-24"))},
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2011-08-30 05:41:09.519"))},
       {"locationIP", boost::any(std::string("81.28.60.168"))},
       {"browser", boost::any(std::string("Internet Explorer"))}});
  auto silva = graph->add_node(
      // 1564|Emperor of Brazil|Silva|female|1984-10-22|2010-01-02T06:04:55.320+0000|192.223.88.63|Chrome
      "Person",
      {{"id", boost::any(1564)},
       {"firstName", boost::any(std::string("Emperor of Brazil"))},
       {"lastName", boost::any(std::string("Silva"))},
       {"gender", boost::any(std::string("female"))},
       {"birthday", boost::any(builtin::datestring_to_int("1984-10-22"))},
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2010-01-02 06:04:55.320"))},
       {"locationIP", boost::any(std::string("192.223.88.63"))},
       {"browser", boost::any(std::string("Chrome"))}});
  auto ivan = graph->add_node(
      // 10995116283243|Ivan Ignatyevich|Aleksandrov|male|1990-01-03|2010-12-25T08:07:55.284+0000|91.149.169.27|Chrome
      "Person",
      {{"id", boost::any(1043)},
       {"firstName", boost::any(std::string("Ivan Ignatyevich"))},
       {"lastName", boost::any(std::string("Aleksandrov"))},
       {"gender", boost::any(std::string("male"))},
       {"birthday", boost::any(builtin::datestring_to_int("1990-01-03"))},
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2010-12-25 08:07:55.284"))},
       {"locationIP", boost::any(std::string("91.149.169.27"))},
       {"browser", boost::any(std::string("Chrome"))}});
  auto bingbing = graph->add_node(
      // 15393162790796|Bingbing|Xu|female|1987-09-19|2011-04-28T23:16:44.375+0000|14.196.249.198|Firefox
      "Person",
      {{"id", boost::any(90796)},
       {"firstName", boost::any(std::string("Bingbing"))},
       {"lastName", boost::any(std::string("Xu"))},
       {"gender", boost::any(std::string("female"))},
       {"birthday", boost::any(builtin::datestring_to_int("1987-09-19"))},
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2011-04-28 23:16:44.375"))},
       {"locationIP", boost::any(std::string("14.196.249.198"))},
       {"browser", boost::any(std::string("Firefox"))}});
  auto Gobi = graph->add_node(
      // id|name|url|type
      // 129|Gobichettipalayam|http://dbpedia.org/resource/Gobichettipalayam|city
      "Place",
      {
          {"id", boost::any(129)},
          {"name", boost::any(std::string("Gobichettipalayam"))},
          {"url", boost::any(std::string(
                      "http://dbpedia.org/resource/Gobichettipalayam"))},
          {"type", boost::any(std::string("city"))},
      });
  auto Kelaniya = graph->add_node(
      // 1353|Kelaniya|http://dbpedia.org/resource/Kelaniya|city
      "Place",
      {
          {"id", boost::any(1353)},
          {"name", boost::any(std::string("Kelaniya"))},
          {"url",
           boost::any(std::string("http://dbpedia.org/resource/Kelaniya"))},
          {"type", boost::any(std::string("city"))},
      });
  auto artux = graph->add_node(
      // 505|Artux|http://dbpedia.org/resource/Artux|city
      "Place",
      {
          {"id", boost::any(505)},
          {"name", boost::any(std::string("Artux"))},
          {"url",
           boost::any(std::string("http://dbpedia.org/resource/Artux"))},
          {"type", boost::any(std::string("city"))},
      });
  auto germany = graph->add_node(
      // 50|Germany|http://dbpedia.org/resource/Germany|country
      "Place",
      {
          {"id", boost::any(50)},
          {"name", boost::any(std::string("Germany"))},
          {"url",
           boost::any(std::string("http://dbpedia.org/resource/Germany"))},
          {"type", boost::any(std::string("country"))},
      });
  auto belarus = graph->add_node(
      // 63|Belarus|http://dbpedia.org/resource/Belarus|country
      "Place",
      {
          {"id", boost::any(63)},
          {"name", boost::any(std::string("Belarus"))},
          {"url",
           boost::any(std::string("http://dbpedia.org/resource/Belarus"))},
          {"type", boost::any(std::string("country"))},
      });
  auto post_13743895 = graph->add_node(
      // id|imageFile|creationDate|locationIP|browserUsed|language|content|length
      // 1374389534791|photo1374389534791.jpg|2011-10-05T14:38:36.019+0000|119.235.7.103|Firefox|||0	
      "Post",
      {
          {"id", boost::any(13743895)}, /* boost::any(std::string("1374389534791"))} */
          {"imageFile", boost::any(std::string("photo1374389534791.jpg"))}, /* String[0..1] */
          {"creationDate",
           boost::any(builtin::dtimestring_to_int("2011-10-05 14:38:36.019"))},
          {"locationIP", boost::any(std::string("119.235.7.103"))},
          {"browser", boost::any(std::string("Firefox"))},
          {"language", boost::any(std::string("uz"))}, /* String[0..1] */ 
          {"content", boost::any(std::string(""))},
          {"length", boost::any(0)}
      });
  auto post_3627 = graph->add_node(
      /* 2061587303627||2012-08-20T09:51:28.275+0000|14.205.203.83|Firefox|uz|About Alexander I of Russia, 
          lexander tried to introduce liberal reforms, while in the second half|98 */
      "Post",
      {
          {"id", boost::any(3627)}, /* boost::any(std::string("2061587303627"))} */
          {"imageFile", boost::any(std::string(""))}, /* String[0..1] */
          {"creationDate",
           boost::any(builtin::dtimestring_to_int("2012-08-20 09:51:28.275"))},
          {"locationIP", boost::any(std::string("14.205.203.83"))},
          {"browser", boost::any(std::string("Firefox"))},
          {"language", boost::any(std::string("uz"))}, /* String[0..1] */ 
          {"content", boost::any(std::string("About Alexander I of Russia, "
            "lexander tried to introduce liberal reforms, while in the second half"))},
          {"length", boost::any(98)}
      });
  auto post_16492674 = graph->add_node(
    /* 1649267442210||2012-01-09T07:50:59.110+0000|192.147.218.174|Internet Explorer|tk|About Louis I of Hungary, dwig der Große,
	      Bulgarian: Лудвиг I, Serbian: Лајош I Анжујски, Czech: Ludvík I. Veliký, Li|117 */
      "Post",
      {{"id", boost::any(16492674)}, //{"id", boost::any(std::string("1649267442210"))},
       //{"type", boost::any(std::string("post"))},
       {"imageFile", boost::any(std::string(""))}, //String[0..1]
       {"language", boost::any(std::string("tk"))}, //String[0..1]
       {"browser", boost::any(std::string("Internet Explorer"))},
       {"locationIP", boost::any(std::string("192.147.218.174"))},
       {"content", boost::any(std::string("About Louis I of Hungary, dwig der Große, Bulgarian: "
        "Лудвиг I, Serbian: Лајош I Анжујски, Czech: Ludvík I. Veliký, Li"))},
       {"length", boost::any(117)},
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2012-01-09 08:05:28.922"))}    
       });
  auto comment_12362343 = graph->add_node(
      // id|creationDate|locationIP|browserUsed|content|length 
      // 1236950581249|2011-08-17T14:26:59.961+0000|92.39.58.88|Chrome|yes|3
      "Comment",
      {
          {"id", boost::any(12362343)},
          //{"type", boost::any(std::string("comment"))},
          {"creationDate", boost::any(builtin::dtimestring_to_int("2011-08-17 14:26:59.961"))},
          {"locationIP", boost::any(std::string("92.39.58.88"))},
          {"browser", boost::any(std::string("Chrome"))},
          {"content", boost::any(std::string("yes"))},
          {"length", boost::any(3)}
      });
  auto comment_16492675 = graph->add_node(
      /* 1649267442211|2012-01-09T08:05:28.922+0000|91.149.169.27|Chrome|About Louis I of Hungary, rchs of the Late Middle Ages,
	        extending terrAbout Louis XIII |87 */
      "Comment",
      {
          {"id", boost::any(16492675)},
          {"creationDate", boost::any(builtin::dtimestring_to_int("2011-10-05 14:38:36.019"))},
          {"locationIP", boost::any(std::string("91.149.169.27"))},
          {"browser", boost::any(std::string("Chrome"))},
          {"content", boost::any(std::string("About Louis I of Hungary, rchs of the Late Middle Ages, "
          "extending terrAbout Louis XIII "))},
          {"length", boost::any(87)}
      });
  auto comment_16492676 = graph->add_node(
      /* 1649267442212|2012-01-10T03:24:33.368+0000|14.196.249.198|Firefox|About Bruce Lee,  sources, in the spirit of his personal
	        martial arts philosophy, whic|86 */
      "Comment",
      {
          {"id", boost::any(16492676)},
          {"creationDate", boost::any(builtin::dtimestring_to_int("2012-01-10 03:24:33.368"))},
          {"locationIP", boost::any(std::string("14.196.249.198"))},
          {"browser", boost::any(std::string("Firefox"))},
          {"content", boost::any(std::string("About Bruce Lee,  sources, in the spirit of "
          "his personal martial arts philosophy, whic"))},
          {"length", boost::any(86)}
      });
  auto comment_16492677 = graph->add_node(
      /* 1649267442213|2012-01-10T14:57:10.420+0000|81.28.60.168|Internet Explorer|I see|5 */ 
      "Comment",
      {
          {"id", boost::any(16492677)},
          {"creationDate", boost::any(builtin::dtimestring_to_int("2012-01-10 14:57:10.420"))},
          {"locationIP", boost::any(std::string("81.28.60.168"))},
          {"browser", boost::any(std::string("Internet Explorer"))},
          {"content", boost::any(std::string("I see"))},
          {"length", boost::any(5)}
      });
  auto comment_1642250 = graph->add_node(
      /* 1649267442250|2012-01-19T11:39:51.385+0000|85.154.120.237|Firefox|About Louis I of Hungary, 
          ittle lasting political results. Louis is theAbout Union of Sou|89  */
      "Comment",
      {
          {"id", boost::any(1642250)},
          {"creationDate", boost::any(builtin::dtimestring_to_int("2012-01-19 11:39:51.385"))},
          {"locationIP", boost::any(std::string("85.154.120.237"))},
          {"browser", boost::any(std::string("Firefox"))},
          {"content", boost::any(std::string("Firefox|About Louis I of Hungary, ittle lasting political results. "
            "Louis is theAbout Union of Sou"))},
          {"length", boost::any(89)}
      });
  auto comment_1642217 = graph->add_node(
      /* 1649267442217|2012-01-10T06:31:18.533+0000|41.76.137.230|Chrome|maybe|5
  */
      "Comment",
      {
          {"id", boost::any(1642217)},
          {"creationDate", boost::any(builtin::dtimestring_to_int("2012-01-10 06:31:18.533"))},
          {"locationIP", boost::any(std::string("41.76.137.230"))},
          {"browser", boost::any(std::string("Chrome"))},
          {"content", boost::any(std::string("maybe"))},
          {"length", boost::any(5)}
      });
  auto forum_37 = graph->add_node(
    // id|title|creationDate 
    // 37|Wall of Hồ Chí Do|2010-02-15T00:46:27.657+0000
      "Forum",
      {{"id", boost::any(37)},
       {"title", boost::any(std::string("Wall of Hồ Chí Do"))},
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2010-02-15 00:46:27.657"))}      
       });
  auto forum_71489 = graph->add_node(
    // 549755871489|Group for Alexander_I_of_Russia in Umeå|2010-09-21T16:25:35.425+0000
      "Forum",
      {{"id", boost::any(71489)},
       {"title", boost::any(std::string("Group for Alexander_I_of_Russia in Umeå"))},
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2010-09-21 16:25:35.425"))}      
       });
  auto tag_206 = graph->add_node(
    // id|name|url
    // 206|Charlemagne|http://dbpedia.org/resource/Charlemagne
      "Tag",
      {{"id", boost::any(206)},
       {"name", boost::any(std::string("Charlemagne"))},
       {"url",
           boost::any(std::string("http://dbpedia.org/resource/Charlemagne"))}     
       });
  auto tag_61 = graph->add_node(
    // 61|Kevin_Rudd|http://dbpedia.org/resource/Kevin_Rudd
      "Tag",
      {{"id", boost::any(61)},
       {"name", boost::any(std::string("Kevin_Rudd"))},
       {"url",
           boost::any(std::string("http://dbpedia.org/resource/Kevin_Rudd"))}     
       });
  auto tag_1679 = graph->add_node(
    // 1679|Alexander_I_of_Russia|http://dbpedia.org/resource/Alexander_I_of_Russia
      "Tag",
      {{"id", boost::any(1679)},
       {"name", boost::any(std::string("Alexander_I_of_Russia"))},
       {"url",
           boost::any(std::string("http://dbpedia.org/resource/Alexander_I_of_Russia"))}     
       });
  auto uni_2213 = graph->add_node( 
    // id|type|name|url
    // 2213|university|Anhui_University_of_Science_and_Technology|
    // http://dbpedia.org/resource/Anhui_University_of_Science_and_Technology 
      "Organisation",
      {{"id", boost::any(2213)},
       {"type", boost::any(std::string("university"))},
       {"name", boost::any(std::string("Anhui_University_of_Science_and_Technology"))},
       {"url",
           boost::any(std::string("http://dbpedia.org/resource/Anhui_University_of_Science_and_Technology"))}     
       });
  auto company_915 = graph->add_node( 
    // id|type|name|url 
    // 915|company|Chang'an_Airlines|http://dbpedia.org/resource/Chang'an_Airlines
      "Organisation",
      {{"id", boost::any(915)},
       {"type", boost::any(std::string("company"))},
       {"name", boost::any(std::string("Chang'an_Airlines"))},
       {"url",
           boost::any(std::string("http://dbpedia.org/resource/Chang'an_Airlines"))}     
       });


  /**
   * Relationships for query interactive short #1
   */
  graph->add_relationship(mahinda, Kelaniya, ":isLocatedIn", {});
  graph->add_relationship(baruch, Gobi, ":isLocatedIn", {});
  

  /**
   * Relationships for query interactive short #3
  	
    Person.id|Person.id|creationDate
	933|4139|2010-03-13T07:37:21.718+0000
	933|6597069777240|2010-09-20T09:42:43.187+0000
	933|10995116284808|2011-01-02T06:43:41.955+0000
	933|32985348833579|2012-09-07T01:11:30.195+0000
	933|32985348838375|2012-07-17T08:04:49.463+0000
   */
  graph->add_relationship(mahinda, baruch, ":KNOWS", {
        {"creationDate", boost::any(builtin::dtimestring_to_int("2010-03-13 07:37:21.718"))}, 
        {"dummy_property", boost::any(std::string("dummy_1"))}});
  graph->add_relationship(mahinda, fritz, ":KNOWS", {
        {"creationDate", boost::any(builtin::dtimestring_to_int("2010-09-20 09:42:43.187"))},
        {"dummy_property", boost::any(std::string("dummy_2"))}});
  graph->add_relationship(mahinda, andrei, ":KNOWS", {
        {"creationDate", boost::any(builtin::dtimestring_to_int("2011-01-02 06:43:41.955"))},
        {"dummy_property", boost::any(std::string("dummy_3"))}});
  graph->add_relationship(mahinda, ottoB, ":KNOWS", {
        {"creationDate", boost::any(builtin::dtimestring_to_int("2012-09-07 01:11:30.195"))},
        {"dummy_property", boost::any(std::string("dummy_4"))}});
  graph->add_relationship(mahinda, ottoR, ":KNOWS", {
        {"creationDate", /* testing date order */ boost::any(builtin::dtimestring_to_int("2012-09-07 01:11:30.195"))},
        {"dummy_property", boost::any(std::string("dummy_5"))}});

  /**
   * Relationships for query interactive short #4

	id|imageFile|creationDate|locationIP|browserUsed|language|content|length
	1374389534791|photo1374389534791.jpg|2011-10-05T14:38:36.019+0000|119.235.7.103|Firefox|||0	
   */

  /**
   * Relationships for query interactive short #5

  	Comment.id|Person.id
  	1236950581249|10995116284808
   */
  graph->add_relationship(comment_12362343, andrei, ":hasCreator", {});

  /**
   * Relationships for query interactive short #6

	Comment.id|Comment.id
	1649267442213|1649267442212 //replyOf
	1649267442212|1649267442211 //replyOf
	
	
	Comment.id|Post.id
	1649267442211|1649267442210 //replyOf	
	
	Forum.id|Post.id
	37|1649267442210 //containerOf
	
	Forum.id|Person.id
	37|4194 //hasModerator
   */
  graph->add_relationship(forum_37, post_16492674, ":containerOf", {});
  graph->add_relationship(forum_37, hoChi, ":hasModerator", {});
  graph->add_relationship(comment_16492675, post_16492674, ":replyOf", {});
  graph->add_relationship(comment_16492676, comment_16492675, ":replyOf", {});
  graph->add_relationship(comment_16492677, comment_16492676, ":replyOf", {});

  /**
   * Relationships for query interactive short #7
   * 
  Comment.id|Comment.id
  1649267442217|1649267442212   // :replyOf

  Comment.id|Person.id
  1649267442217|15393162795439    // :hasCreator
  1649267442213|19791209307382

   */
  graph->add_relationship(comment_1642217, comment_16492676, ":replyOf", {});
  graph->add_relationship(comment_1642217, lomana, ":hasCreator", {});
  graph->add_relationship(comment_16492677, amin, ":hasCreator", {});
  graph->add_relationship(comment_16492676, bingbing, ":hasCreator", {});
  graph->add_relationship(lomana, bingbing, ":KNOWS", {});

  /**
   * Relationships for query update #1

  Person.id|Organisation.id|classYear
  2370|2213|2001

  Person.id|Organisation.id|workFrom
  2370|915|2002

  Person.id|Place.id
  2370|505

  Person.id|Tag.id
  2370|61
   */

  /**
   * Relationships for query update #2
   */

  /**
   * Relationships for query update #3

  Person.id|Comment.id|creationDate
  1564|1649267442250|2012-01-23T08:56:30.617+0000
   */

  /**
   * Relationships for query update #4

  Forum.id|Person.id
  53975|1564  // :hasModerator

  Forum.id|Tag.id
  53975|206   // :hasTag

  id|name|url
  206|Charlemagne|http://dbpedia.org/resource/Charlemagne

  id|title|creationDate 
  53975|Wall of Emperor of Brazil Silva|2010-01-02T06:05:05.320+0000
   */

  /**
   * Relationships for query update #5

  Forum.id|Person.id|joinDate
  37|1564|2010-02-23T09:10:25.466+0000  
   */

  /**
   * Relationships for query update #6

  Post.id|Person.id
  1374392536304|6597069777240

  Forum.id|Post.id
  549755871489|1374392536304

  Post.id|Place.id
  1374392536304|50

  Post.id|Tag.id
  1374392536304|1679
   */

  /**
   * Relationships for query update #7

  Comment.id|Person.id
  1649267442214|10995116283243

  Comment.id|Post.id
  1649267442214|1649267442210

  Comment.id|Place.id
  1649267442214|63

  Post.id|Tag.id
  1649267442214|1679
   */

  /**
   * Relationships for query update #8

  Person.id|Person.id|creationDate
  1564|4194|2010-02-23T09:10:15.466+0000
   */

#ifdef USE_TX
  graph->commit_transaction();
#endif

  return graph;
}



graph_db_ptr create_graph2(
#ifdef USE_PMDK
    nvm::pool_base &pop
#endif
) {
#ifdef USE_PMDK
  graph_db_ptr graph;
  nvm::transaction::run(pop, [&] { graph = p_make_ptr<graph_db>(); });
#else
  auto graph = p_make_ptr<graph_db>();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  auto ravalomanana = graph->add_node(
      // 65|Marc|Ravalomanana|female|1989-06-15|2010-02-26T23:17:18.465+0000|41.204.119.20|Firefox
      "Person",
      {{"id", boost::any(65)},
       {"firstName", boost::any(std::string("Marc"))},
       {"lastName", boost::any(std::string("Ravalomanana"))},
       {"gender", boost::any(std::string("female"))},
       {"birthday", boost::any(builtin::datestring_to_int("1989-06-15"))},
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2010-02-26 23:17:18.465"))},
       {"locationIP", boost::any(std::string("41.204.119.20"))},
       {"browser", boost::any(std::string("Firefox"))}});
  auto person2_1 = graph->add_node(
      // 19791209302379|Muhammad|Iqbal|female|1983-09-13|2011-08-14T03:06:21.524+0000|202.14.71.199|Chrome
      "Person",
      {{"id", boost::any(1379)},
       {"firstName", boost::any(std::string("Muhammad"))},
       {"lastName", boost::any(std::string("Iqbal"))},
       {"gender", boost::any(std::string("female"))},
       {"birthday", boost::any(builtin::datestring_to_int("1983-09-13"))},
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2011-08-14 03:06:21.524"))},
       {"locationIP", boost::any(std::string("202.14.71.199"))},
       {"browser", boost::any(std::string("Chrome"))}});
  auto person2_2 = graph->add_node(
      // 17592186055291|Wei|Li|female|1986-09-24|2011-05-10T20:09:44.151+0000|1.4.4.26|Chrome
      "Person",
      {{"id", boost::any(1291)},
       {"firstName", boost::any(std::string("Wei"))},
       {"lastName", boost::any(std::string("Li"))},
       {"gender", boost::any(std::string("female"))},
       {"birthday", boost::any(builtin::datestring_to_int("1986-09-24"))},
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2011-05-10 20:09:44.151"))},
       {"locationIP", boost::any(std::string("1.4.4.26"))},
       {"browser", boost::any(std::string("Chrome"))}});
  auto person2_3 = graph->add_node(
      // 15393162799121|Karl|Beran|male|1983-05-30|2011-04-02T00:14:40.528+0000|31.130.85.235|Chrome
      "Person",
      {{"id", boost::any(1121)},
       {"firstName", boost::any(std::string("Karl"))},
       {"lastName", boost::any(std::string("Beran"))},
       {"gender", boost::any(std::string("male"))},
       {"birthday", boost::any(builtin::datestring_to_int("1983-05-30"))},
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2011-04-02 00:14:40.528"))},
       {"locationIP", boost::any(std::string("31.130.85.235"))},
       {"browser", boost::any(std::string("Chrome"))}});
  auto person2_4 = graph->add_node(
      // 8796093028680|Rahul|Singh|female|1981-12-29|2010-10-09T07:08:12.913+0000|61.17.209.13|Firefox
      "Person",
      {{"id", boost::any(1680)},
       {"firstName", boost::any(std::string("Rahul"))},
       {"lastName", boost::any(std::string("Singh"))},
       {"gender", boost::any(std::string("female"))},
       {"birthday", boost::any(builtin::datestring_to_int("1981-12-2"))},
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2010-10-09 07:08:12.913"))},
       {"locationIP", boost::any(std::string("61.17.209.13"))},
       {"browser", boost::any(std::string("Firefox"))}});
  auto person2_5 = graph->add_node(
      // 19791209302377|John|Smith|male|1983-08-31|2011-08-10T15:59:24.890+0000|24.245.233.94|Firefox
      "Person",
      {{"id", boost::any(1377)},
       {"firstName", boost::any(std::string("John"))},
       {"lastName", boost::any(std::string("Smith"))},
       {"gender", boost::any(std::string("male"))},
       {"birthday", boost::any(builtin::datestring_to_int("1983-08-31"))},
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2011-08-10 15:59:24.890"))},
       {"locationIP", boost::any(std::string("24.245.233.94"))},
       {"browser", boost::any(std::string("Firefox"))}});
  auto person2_6 = graph->add_node(
      // 10995116278350|Abdul|Aman|male|1982-05-24|2010-11-17T00:16:33.065+0000|180.222.141.92|Internet Explorer
      "Person",
      {{"id", boost::any(1350)},
       {"firstName", boost::any(std::string("Abdul"))},
       {"lastName", boost::any(std::string("Aman"))},
       {"gender", boost::any(std::string("male"))},
       {"birthday", boost::any(builtin::datestring_to_int("1982-05-24"))},
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2010-11-17 00:16:33.065"))},
       {"locationIP", boost::any(std::string("180.222.141.92"))},
       {"browser", boost::any(std::string("Internet Explorer"))}});
  auto person2_7 = graph->add_node(
      // 4398046514661|Rajiv|Singh|male|1983-02-17|2010-05-13T06:57:29.021+0000|49.46.196.167|Firefox
      "Person",
      {{"id", boost::any(1661)},
       {"firstName", boost::any(std::string("Rajiv"))},
       {"lastName", boost::any(std::string("Singh"))},
       {"gender", boost::any(std::string("male"))},
       {"birthday", boost::any(builtin::datestring_to_int("1983-02-17"))},
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2010-05-13 06:57:29.021"))},
       {"locationIP", boost::any(std::string("49.46.196.167"))},
       {"browser", boost::any(std::string("Firefox"))}});
  auto person2_8 = graph->add_node(
      // 10995116278353|Otto|Muller|male|1988-10-28|2010-12-19T22:06:54.592+0000|204.79.148.6|Firefox
      "Person",
      {{"id", boost::any(18353)},
       {"firstName", boost::any(std::string("Otto"))},
       {"lastName", boost::any(std::string("Muller"))},
       {"gender", boost::any(std::string("male"))},
       {"birthday", boost::any(builtin::datestring_to_int("1988-10-28"))},
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2010-12-19 22:06:54.592"))},
       {"locationIP", boost::any(std::string("204.79.148.6"))},
       {"browser", boost::any(std::string("Firefox"))}});


  auto post2_1 = graph->add_node(
    // id|imageFile|creationDate|locationIP|browserUsed|language|content|length
    /* 1374390164863||2011-10-17T05:40:34.561+0000|41.204.119.20|Firefox|uz|About Paul Keres,  in 
        the Candidates' Tournament on four consecutive occasions. Due to these and other strong results, 
        many chess historians consider Keres the strongest player never to be|188 */
      "Post",
      {{"id", boost::any(1863)}, 
       {"imageFile", boost::any(std::string(""))}, //String[0..1]
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2011-10-17 05:40:34.561"))},
       {"locationIP", boost::any(std::string("41.204.119.20"))},
       {"browser", boost::any(std::string("Firefox"))},
       {"language", boost::any(std::string("\"uz\""))}, //String[0..1]       
       {"content", boost::any(std::string("About Paul Keres,  in the "
       "Candidates' Tournament on four consecutive occasions. Due to these "
       "and other strong results, many chess historians consider Keres the strongest player never to be"))},
       {"length", boost::any(188)}           
       });
  auto post2_2 = graph->add_node(
    /* 1649268071976||2012-01-14T09:41:00.992+0000|24.245.233.94|Firefox|uz|About Paul Keres, hampionship "
      match against champion Alexander Alekhine, but the match never took place due to World War|120 */
      "Post",
      {{"id", boost::any(1976)}, 
       {"imageFile", boost::any(std::string(""))}, //String[0..1]
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2012-01-14 09:41:00.992"))},
       {"locationIP", boost::any(std::string("24.245.233.94"))},
       {"browser", boost::any(std::string("Firefox"))},
       {"language", boost::any(std::string("\"uz\""))}, //String[0..1]       
       {"content", boost::any(std::string("About Paul Keres, hampionship "
        "match against champion Alexander Alekhine, but the match never took place due to World War"))},
       {"length", boost::any(120)}           
       });
  auto post2_3 = graph->add_node(
    /* 1374390165125||2011-10-16T15:05:23.955+0000|204.79.148.6|Firefox|uz|About Otto von Bismarck, onsible 
    for the unifiAbout Muammar Gaddafi, e styled himself as LAbout Pete T|102 */
      "Post",
      {{"id", boost::any(1125)}, 
       {"imageFile", boost::any(std::string(""))}, //String[0..1]
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2011-10-16 15:05:23.955"))},
       {"locationIP", boost::any(std::string("204.79.148.6"))},
       {"browser", boost::any(std::string("Firefox"))},
       {"language", boost::any(std::string("\"uz\""))}, //String[0..1]       
       {"content", boost::any(std::string("About Otto von Bismarck, onsible "
        "for the unifiAbout Muammar Gaddafi, e styled himself as LAbout Pete T"))},
       {"length", boost::any(188)}           
       });
  auto post2_4 = graph->add_node(
    /* 1374390165164||2011-10-16T23:30:53.955+0000|1.4.4.26|Chrome|uz|About Muammar Gaddafi, June 1942Sirte, 
    Libya Died 20 October 2About James Joyce, he Jesuit schools Clongowes and BelvedeAbout Laurence Olivier,  and 
    British drama. He was the first arAbout Osa|192 */
      "Post",
      {{"id", boost::any(1164)}, 
       {"imageFile", boost::any(std::string(""))}, //String[0..1]
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2011-10-16 23:30:53.955"))},
       {"locationIP", boost::any(std::string("1.4.4.26"))},
       {"browser", boost::any(std::string("Chrome"))},
       {"language", boost::any(std::string("\"uz\""))}, //String[0..1]       
       {"content", boost::any(std::string("About Muammar Gaddafi, June 1942Sirte, "
        "Libya Died 20 October 2About James Joyce, he Jesuit schools Clongowes and BelvedeAbout Laurence Olivier,  and "
        "British drama. He was the first arAbout Osa"))},
       {"length", boost::any(192)}           
       });
  auto post2_5 = graph->add_node(
    /* 1786712928767||2012-03-29T11:17:50.625+0000|31.130.85.235|Chrome|ar|About Catherine the 
    Great, (2 May  1729 – 17 November  1796), was the most renowned and th|90 */
      "Post",
      {{"id", boost::any(1767)}, 
       {"imageFile", boost::any(std::string(""))}, //String[0..1]
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2012-03-29 11:17:50.625"))},
       {"locationIP", boost::any(std::string("31.130.85.235"))},
       {"browser", boost::any(std::string("Chrome"))},
       {"language", boost::any(std::string("\"ar\""))}, //String[0..1]       
       {"content", boost::any(std::string("About Catherine the "
        "Great, (2 May  1729 – 17 November  1796), was the most renowned and th"))},
       {"length", boost::any(90)}           
       });
  auto post2_6 = graph->add_node(
    /* 1099518161705||2011-06-24T22:26:11.884+0000|61.17.209.13|Firefox|ar|About Catherine the Great, 
    e largest share. In the east, Russia started to colonise Al|86 */
      "Post",
      {{"id", boost::any(1705)}, 
       {"imageFile", boost::any(std::string(""))}, //String[0..1]
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2011-06-24 22:26:11.884"))},
       {"locationIP", boost::any(std::string("61.17.209.13"))},
       {"browser", boost::any(std::string("Firefox"))},
       {"language", boost::any(std::string("\"ar\""))}, //String[0..1]       
       {"content", boost::any(std::string("About Catherine the Great, "
        "e largest share. In the east, Russia started to colonise Al"))},
       {"length", boost::any(86)}           
       });
  auto post2_7 = graph->add_node(
    /* 1374396068816||2011-09-27T05:59:43.468+0000|49.46.196.167|Firefox|ar|About Fernando González, 
    y Chile's best tennis player oAbout Vichy France,  (GPRF). Most |89 */
      "Post",
      {{"id", boost::any(1816)}, 
       {"imageFile", boost::any(std::string(""))}, //String[0..1]
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2011-09-27 05:59:43.468"))},
       {"locationIP", boost::any(std::string("49.46.196.167"))},
       {"browser", boost::any(std::string("Firefox"))},
       {"language", boost::any(std::string("\"ar\""))}, //String[0..1]       
       {"content", boost::any(std::string("About Fernando González, "
        "y Chile's best tennis player oAbout Vichy France,  (GPRF). Most "))},
       {"length", boost::any(89)}           
       });
  auto post2_8 = graph->add_node(
    /* 1374396068820||2011-09-26T16:39:28.468+0000|49.46.196.167|Firefox|ar|About Fernando González, ian Open, 
    losing tAbout Mary, Queen of Scots, ter. In 1558, she About David, to p|106 */
      "Post",
      {{"id", boost::any(1816)}, 
       {"imageFile", boost::any(std::string(""))}, //String[0..1]
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2011-09-26 16:39:28.468"))},
       {"locationIP", boost::any(std::string("49.46.196.167"))},
       {"browser", boost::any(std::string("Firefox"))},
       {"language", boost::any(std::string("\"ar\""))}, //String[0..1]       
       {"content", boost::any(std::string("About Fernando González, ian Open, "
        "losing tAbout Mary, Queen of Scots, ter. In 1558, she About David, to p"))},
       {"length", boost::any(106)}           
       });
  auto post2_9 = graph->add_node(
    /* 1374396068835||2011-09-27T09:25:13.468+0000|49.46.196.167|Firefox|ar|About Fernando González, 
    en, Marat SafiAbout Cole Porter, u Under My SkiAbout Ray Bradbury, 953) and for tA|107 */
      "Post",
      {{"id", boost::any(1835)}, 
       {"imageFile", boost::any(std::string(""))}, //String[0..1]
       {"creationDate",
        boost::any(builtin::dtimestring_to_int("2011-09-27 09:25:13.468"))},
       {"locationIP", boost::any(std::string("49.46.196.167"))},
       {"browser", boost::any(std::string("Firefox"))},
       {"language", boost::any(std::string("\"ar\""))}, //String[0..1]       
       {"content", boost::any(std::string("About Fernando González, "
        "en, Marat SafiAbout Cole Porter, u Under My SkiAbout Ray Bradbury, 953) and for tA"))},
       {"length", boost::any(107)}           
       });
  auto comment2_1 = graph->add_node(
      /* 1374390164865|2011-10-17T09:17:43.567+0000|41.204.119.20|Firefox|About Paul Keres, Alexander Alekhine, 
      but the match never toAbout Birth of a Prince|83 */
      "Comment",
      {
          {"id", boost::any(1865)},
          {"creationDate", boost::any(builtin::dtimestring_to_int("2013-01-17 09:17:43.567"))},
          {"locationIP", boost::any(std::string("41.204.119.20"))},
          {"browser", boost::any(std::string("Firefox"))},
          {"content", boost::any(std::string("About Paul Keres, Alexander Alekhine, "
            "but the match never toAbout Birth of a Prince"))},
          {"length", boost::any(83)}
      });
  auto comment2_2 = graph->add_node(
      /* 1374390164877|2011-10-17T10:59:33.177+0000|41.204.119.20|Firefox|yes|3 */
      "Comment",
      {
          {"id", boost::any(1877)},
          {"creationDate", boost::any(builtin::dtimestring_to_int("2013-2-17 10:59:33.177"))},
          {"locationIP", boost::any(std::string("41.204.119.20"))},
          {"browser", boost::any(std::string("Firefox"))},
          {"content", boost::any(std::string("yes"))},
          {"length", boost::any(3)}
      });
  auto comment2_3 = graph->add_node(
      /* 1649268071978|2012-01-14T16:57:46.045+0000|41.204.119.20|Firefox|yes|3 */
      "Comment",
      {
          {"id", boost::any(1978)},
          {"creationDate", boost::any(builtin::dtimestring_to_int("2013-03-14 16:57:46.045"))},
          {"locationIP", boost::any(std::string("41.204.119.20"))},
          {"browser", boost::any(std::string("Firefox"))},
          {"content", boost::any(std::string("yes"))},
          {"length", boost::any(3)}
      });
  auto comment2_4 = graph->add_node(
      /* 1374390165126|2011-10-16T21:16:03.354+0000|41.204.119.20|Firefox|roflol|6 */
      "Comment",
      {
          {"id", boost::any(1126)},
          {"creationDate", boost::any(builtin::dtimestring_to_int("2013-04-16 21:16:03.354"))},
          {"locationIP", boost::any(std::string("41.204.119.20"))},
          {"browser", boost::any(std::string("Firefox"))},
          {"content", boost::any(std::string("roflol"))},
          {"length", boost::any(6)}
      });
  auto comment2_5 = graph->add_node(
      /* 1374390165171|2011-10-17T19:37:26.339+0000|41.204.119.20|Firefox|yes|3 */
      "Comment",
      {
          {"id", boost::any(1171)},
          {"creationDate", boost::any(builtin::dtimestring_to_int("2013-05-17 19:37:26.339"))},
          {"locationIP", boost::any(std::string("41.204.119.20"))},
          {"browser", boost::any(std::string("Firefox"))},
          {"content", boost::any(std::string("yes"))},
          {"length", boost::any(3)}
      });
  auto comment2_6 = graph->add_node(
      /* 1786712928768|2012-03-29T17:57:51.844+0000|41.204.119.20|Firefox|LOL|3 */
      "Comment",
      {
          {"id", boost::any(1768)},
          {"creationDate", boost::any(builtin::dtimestring_to_int("2013-06-29 17:57:51.844"))},
          {"locationIP", boost::any(std::string("41.204.119.20"))},
          {"browser", boost::any(std::string("Firefox"))},
          {"content", boost::any(std::string("LOL"))},
          {"length", boost::any(3)}
      });
  auto comment2_7 = graph->add_node(
      /* 1099518161711|2011-06-25T07:54:01.976+0000|41.204.119.20|Firefox|no|2 */
      "Comment",
      {
          {"id", boost::any(1711)},
          {"creationDate", boost::any(builtin::dtimestring_to_int("2013-07-25 07:54:01.976"))},
          {"locationIP", boost::any(std::string("41.204.119.20"))},
          {"browser", boost::any(std::string("Firefox"))},
          {"content", boost::any(std::string("no"))},
          {"length", boost::any(2)}
      });
  auto comment2_8 = graph->add_node(
      /* 1099518161722|2011-06-25T12:56:57.280+0000|41.204.119.20|Firefox|LOL|3 */
      "Comment",
      {
          {"id", boost::any(1722)},
          {"creationDate", boost::any(builtin::dtimestring_to_int("2013-08-25 12:56:57.280"))},
          {"locationIP", boost::any(std::string("41.204.119.20"))},
          {"browser", boost::any(std::string("Firefox"))},
          {"content", boost::any(std::string("LOL"))},
          {"length", boost::any(3)}
      });
  auto comment2_9 = graph->add_node(
      /* 1374396068819|2011-09-27T09:41:01.413+0000|41.204.119.20|Firefox|About Fernando González, 
      er from Chile. He is kAbout George W. Bush, 04 for a description oAbout Vichy Franc|108 */
      "Comment",
      {
          {"id", boost::any(1819)},
          {"creationDate", boost::any(builtin::dtimestring_to_int("2013-09-27 09:41:01.413"))},
          {"locationIP", boost::any(std::string("41.204.119.20"))},
          {"browser", boost::any(std::string("Firefox"))},
          {"content", boost::any(std::string("About Fernando González, "
            "er from Chile. He is kAbout George W. Bush, 04 for a description oAbout Vichy Franc"))},
          {"length", boost::any(108)}
      });
  auto comment2_10 = graph->add_node(
      /* 1374396068821|2011-09-26T23:46:18.580+0000|41.76.205.156|Firefox|About Fernando González, 
      les at Athens 2004About Ronald Reagan, st in films and laAbou|86 */
      "Comment",
      {
          {"id", boost::any(1821)},
          {"creationDate", boost::any(builtin::dtimestring_to_int("2013-10-26 23:46:18.580"))},
          {"locationIP", boost::any(std::string("41.76.205.156"))},
          {"browser", boost::any(std::string("Firefox"))},
          {"content", boost::any(std::string("About Fernando González, "
            "les at Athens 2004About Ronald Reagan, st in films and laAbou"))},
          {"length", boost::any(86)}
      });
  auto comment2_11 = graph->add_node(
      /* 1374396068827|2011-09-26T17:09:07.283+0000|41.204.119.20|Firefox|About Fernando González, 
      Safin, and Pete SAbout Mary, Queen of Scots,  had previously cAbout Edward the Confessor, isintegration of Abo|135 */
      "Comment",
      {
          {"id", boost::any(1827)},
          {"creationDate", boost::any(builtin::dtimestring_to_int("2013-11-26 17:09:07.283"))},
          {"locationIP", boost::any(std::string("41.204.119.20"))},
          {"browser", boost::any(std::string("Firefox"))},
          {"content", boost::any(std::string("About Fernando González, "
            "Safin, and Pete SAbout Mary, Queen of Scots,  had previously cAbout Edward the Confessor, isintegration of Abo"))},
          {"length", boost::any(135)}
      });
  auto comment2_12 = graph->add_node(
      /* 1374396068837|2011-09-27T11:32:19.336+0000|41.204.119.20|Firefox|maybe|5 */
      "Comment",
      {
          {"id", boost::any(1837)},
          {"creationDate", boost::any(builtin::dtimestring_to_int("2013-12-27 11:32:19.336"))},
          {"locationIP", boost::any(std::string("41.204.119.20"))},
          {"browser", boost::any(std::string("Firefox"))},
          {"content", boost::any(std::string("maybe"))},
          {"length", boost::any(5)}
      });

  /**
   * Relationships for query interactive short #2

  Comment.id|Person.id
  1099518161711|65
  1099518161722|65
  1374390164865|65
  1374390164877|65
  1374390165126|65
  1374390165171|65
  1374396068819|65
  1374396068821|65
  1374396068827|65
  1374396068837|65
  1649268071978|65
  1786712928768|65


  Comment.id|Post.id
  1374390164865|1374390164863
  1374390164877|1374390164863
  1649268071978|1649268071976
  1374390165126|1374390165125
  1374390165171|1374390165164
  1786712928768|1786712928767
  1099518161711|1099518161705
  1099518161722|1099518161705
  1374396068819|1374396068816
  1374396068821|1374396068820
  1374396068827|1374396068820
  1374396068837|1374396068835

  Post.id|Person.id
  1374390164863|65
  1649268071976|19791209302377
  1374390165125|10995116278353
  1374390165164|17592186055291
  1786712928767|15393162799121
  1099518161705|8796093028680
  1374396068816|4398046514661
  1374396068820|4398046514661
  1374396068835|4398046514661
   */
  graph->add_relationship(comment2_1, ravalomanana, ":hasCreator", {});
  graph->add_relationship(comment2_2, ravalomanana, ":hasCreator", {});
  graph->add_relationship(comment2_3, ravalomanana, ":hasCreator", {});
  graph->add_relationship(comment2_4, ravalomanana, ":hasCreator", {});
  graph->add_relationship(comment2_5, ravalomanana, ":hasCreator", {});
  graph->add_relationship(comment2_6, ravalomanana, ":hasCreator", {});
  graph->add_relationship(comment2_7, ravalomanana, ":hasCreator", {});
  graph->add_relationship(comment2_8, ravalomanana, ":hasCreator", {});
  graph->add_relationship(comment2_9, ravalomanana, ":hasCreator", {});
  graph->add_relationship(comment2_10, ravalomanana, ":hasCreator", {});
  graph->add_relationship(comment2_11, ravalomanana, ":hasCreator", {});
  graph->add_relationship(comment2_12, ravalomanana, ":hasCreator", {});

  graph->add_relationship(comment2_1, post2_1, ":replyOf", {});
  graph->add_relationship(comment2_2, post2_1, ":replyOf", {});
  graph->add_relationship(comment2_3, post2_2, ":replyOf", {});
  graph->add_relationship(comment2_4, post2_3, ":replyOf", {});
  graph->add_relationship(comment2_5, post2_4, ":replyOf", {});
  graph->add_relationship(comment2_6, post2_5, ":replyOf", {});
  graph->add_relationship(comment2_7, post2_6, ":replyOf", {});
  graph->add_relationship(comment2_8, post2_6, ":replyOf", {});
  graph->add_relationship(comment2_9, post2_7, ":replyOf", {});
  graph->add_relationship(comment2_10, post2_8, ":replyOf", {});
  graph->add_relationship(comment2_11, post2_8, ":replyOf", {});
  graph->add_relationship(comment2_12, post2_9, ":replyOf", {});

  graph->add_relationship(post2_1, ravalomanana, ":hasCreator", {});
  graph->add_relationship(post2_2, person2_5, ":hasCreator", {});
  graph->add_relationship(post2_3, person2_8, ":hasCreator", {});
  graph->add_relationship(post2_4, person2_2, ":hasCreator", {});
  graph->add_relationship(post2_5, person2_3, ":hasCreator", {});
  graph->add_relationship(post2_6, person2_4, ":hasCreator", {});
  graph->add_relationship(post2_7, person2_7, ":hasCreator", {});
  graph->add_relationship(post2_8, person2_7, ":hasCreator", {});
  graph->add_relationship(post2_9, person2_7, ":hasCreator", {});

#ifdef USE_TX
  graph->commit_transaction();
#endif

  return graph;
}

void run_all_ldbc_queries(graph_db_ptr &gdb, graph_db_ptr &gdb2);

/* interactive short queries */
void run_ldbc_is_query_1(graph_db_ptr &gdb, result_set &rs);
void run_ldbc_is_query_2(graph_db_ptr &gdb, result_set &rs);
void run_ldbc_is_query_3(graph_db_ptr &gdb, result_set &rs);
void run_ldbc_is_query_4(graph_db_ptr &gdb, result_set &rs);
void run_ldbc_is_query_5(graph_db_ptr &gdb, result_set &rs);
void run_ldbc_is_query_6(graph_db_ptr &gdb, result_set &rs);
void run_ldbc_is_query_7(graph_db_ptr &gdb, result_set &rs);


/* interactive update queries */
void run_ldbc_iu_query_1(graph_db_ptr &gdb, result_set &rs);
void run_ldbc_iu_query_2(graph_db_ptr &gdb, result_set &rs);
void run_ldbc_iu_query_3(graph_db_ptr &gdb, result_set &rs);
void run_ldbc_iu_query_4(graph_db_ptr &gdb, result_set &rs);
void run_ldbc_iu_query_5(graph_db_ptr &gdb, result_set &rs);
void run_ldbc_iu_query_6(graph_db_ptr &gdb, result_set &rs);
void run_ldbc_iu_query_7(graph_db_ptr &gdb, result_set &rs);
void run_ldbc_iu_query_8(graph_db_ptr &gdb, result_set &rs);


/**--------------------------------------------------------------------------------
 * ------------------------- LDBC TESTS -------------------------------------------
 * --------------------------------------------------------------------------------
 * --------------------------------------------------------------------------------
 * */


/*TEST_CASE("Testing LDBC interactive short queries", "[ldbc]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto graph = create_graph1(pop);
  auto graph2 = create_graph2(pop);
#else
  auto graph = create_graph1();
  auto graph2 = create_graph2();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  
  SECTION("query interactive short #1") {
    result_set rs, expected;

    // firstName, lastName, birthday, locationIP, browser, placeId, gender
    // Mahinda|Perera|1989-12-03|119.235.7.103|Firefox|1353|male
    expected.data.push_back(
        {query_result("Mahinda"), query_result("Perera"),
         query_result("1989-12-03"), query_result("119.235.7.103"),
         query_result("Firefox"), query_result("1353"), query_result("male"),
         query_result("2010-Feb-14 15:32:10")});

    run_ldbc_is_query_1(graph, rs);

    REQUIRE(rs == expected);
  }

  SECTION("query interactive short #2") {
    result_set rs, expected;

    expected.data.push_back(
        {query_result("1863"), 
        query_result("About Paul Keres,  in the Candidates' Tournament on four consecutive "
            "occasions. Due to these and other strong results, many chess historians consider "
            "Keres the strongest player never to be"),
        query_result("2011-Oct-17 05:40:34"), query_result("1863"), query_result("65"),
        query_result("Marc"), query_result("Ravalomanana")});
    expected.data.push_back({
        query_result("1837"), query_result("maybe"), query_result("2013-Dec-27 11:32:19"), 
        query_result("1835"), query_result("1661"), query_result("Rajiv"), query_result("Singh")});
    expected.data.push_back({
        query_result("1827"), query_result("About Fernando González, Safin, and Pete SAbout Mary, Queen of "
            "Scots,  had previously cAbout Edward the Confessor, isintegration of Abo"), 
        query_result("2013-Nov-26 17:09:07"), query_result("1816"), query_result("1661"), 
        query_result("Rajiv"), query_result("Singh")});
    expected.data.push_back({
        query_result("1821"), query_result("About Fernando González, les at Athens 2004About Ronald Reagan, st "
            "in films and laAbou"), 
        query_result("2013-Oct-26 23:46:18"), query_result("1816"), query_result("1661"), 
        query_result("Rajiv"), query_result("Singh")});
    expected.data.push_back({
        query_result("1819"), query_result("About Fernando González, er from Chile. He is kAbout George W. Bush, "
            "04 for a description oAbout Vichy Franc"), 
        query_result("2013-Sep-27 09:41:01"), query_result("1816"), query_result("1661"), 
        query_result("Rajiv"), query_result("Singh")});
    expected.data.push_back({
        query_result("1722"), query_result("LOL"), query_result("2013-Aug-25 12:56:57"), 
        query_result("1705"), query_result("1680"), query_result("Rahul"), query_result("Singh")});
    expected.data.push_back({
        query_result("1711"), query_result("no"), query_result("2013-Jul-25 07:54:01"), 
        query_result("1705"), query_result("1680"), query_result("Rahul"), query_result("Singh")});
    expected.data.push_back({
        query_result("1768"), query_result("LOL"), query_result("2013-Jun-29 17:57:51"), 
        query_result("1767"), query_result("1121"), query_result("Karl"), query_result("Beran")});
    expected.data.push_back({
        query_result("1171"), query_result("yes"), query_result("2013-May-17 19:37:26"), 
        query_result("1164"), query_result("1291"), query_result("Wei"), query_result("Li")});
    expected.data.push_back({
        query_result("1126"), query_result("roflol"), query_result("2013-Apr-16 21:16:03"), 
        query_result("1125"), query_result("18353"), query_result("Otto"), query_result("Muller")});

    run_ldbc_is_query_2(graph2, rs);
    //std::cout << rs;

    REQUIRE(rs == expected);
  }

  SECTION("query interactive short #3") {
    result_set rs, expected;

    expected.data.push_back(
        {query_result("833579"), query_result("Otto"),
         query_result("Becker"), query_result("2012-Sep-07 01:11:30")});
    expected.data.push_back(
        {query_result("838375"), query_result("Otto"),
         query_result("Richter"), query_result("2012-Sep-07 01:11:30")});
    expected.data.push_back(
        {query_result("10995116"), query_result("Andrei"),
         query_result("Condariuc"), query_result("2011-Jan-02 06:43:41")});
    expected.data.push_back(
        {query_result("65970697"), query_result("Fritz"),
         query_result("Muller"), query_result("2010-Sep-20 09:42:43")});
    expected.data.push_back(
        {query_result("4139"), query_result("Baruch"),
         query_result("Dego"), query_result("2010-Mar-13 07:37:21")});
    
    run_ldbc_is_query_3(graph, rs);
    //std::cout << rs;

    REQUIRE(rs == expected);
  }

  SECTION("query interactive short #4") {
    result_set rs, expected;

    expected.data.push_back(
        {query_result("2011-Oct-05 14:38:36"),
         query_result("photo1374389534791.jpg")});
    

    run_ldbc_is_query_4(graph, rs);
    //std::cout << rs;

    REQUIRE(rs == expected);
  }

  SECTION("query interactive short #5") {
    result_set rs, expected;

    expected.data.push_back(
        {query_result("10995116"),
         query_result("Andrei"),
         query_result("Condariuc")});
    

    run_ldbc_is_query_5(graph, rs);
    //std::cout << rs;

    REQUIRE(rs == expected);
  }

  SECTION("query interactive short #6") {
    result_set rs, expected;

    expected.data.push_back(
        {query_result("37"),
         query_result("Wall of Hồ Chí Do"),
         query_result("4194"),
         query_result("Hồ Chí"),
         query_result("Do")});

    run_ldbc_is_query_6(graph, rs);
    //std::cout << rs;
    
    REQUIRE(rs == expected);
  }

  SECTION("query interactive short #7") {
    result_set rs, expected;

    expected.data.push_back(
        {query_result("16492677"),
         query_result("I see"),
         query_result("2012-Jan-10 14:57:10"),
         query_result("19791"),
         query_result("Amin"),
         query_result("Kamkar"),
         query_result("false")});

    expected.data.push_back(
        {query_result("1642217"),
         query_result("maybe"),
         query_result("2012-Jan-10 06:31:18"),
         query_result("15393"),
         query_result("Lomana Trésor"),
         query_result("Kanam"),
         query_result("true")});

    run_ldbc_is_query_7(graph, rs);
    //std::cout << rs;
    
    REQUIRE(rs == expected);
  }


#ifdef USE_TX
  graph->commit_transaction();
#endif

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph2); });
  pop.close();
  remove(test_path.c_str());
#endif
}


TEST_CASE("Testing LDBC interactive update queries", "[ldbc]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto graph = create_graph1(pop);
#else
  auto graph = create_graph1();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif


  SECTION("query update #1") {
    result_set rs, expected;

    expected.append(
        {query_result("Person[34]{birthday: 383097600, browser: \"Internet Explorer\", "
                      "creationDate: 1263715167, email: \"\"Yang2370@gmail.com\", \"Yang2370@hotmail.com\"\", firstName: \"Yang\", "
                      "gender: \"male\", id: 2370, language: \"\"zh\", \"en\"\", lastName: \"Zhu\", "
                      "locationIP: \"1.183.127.173\"}"),
        query_result("Place[15]{id: 505, name: \"Artux\", type: \"city\", url: \"http://dbpedia.org/resource/Artux\"}"),
        query_result("::isLocatedIn[18]{}"),
        query_result("Tag[30]{id: 61, name: \"Kevin_Rudd\", url: \"http://dbpedia.org/resource/Kevin_Rudd\"}"),
        query_result("::hasInterest[19]{}"),
        query_result("Organisation[32]{id: 2213, name: \"Anhui_University_of_Science_and_Technology\", type: \"university\", "
                      "url: \"http://dbpedia.org/resource/Anhui_University_of_Science_and_Technology\"}"),
        query_result("::studyAt[20]{classYear: 2001}"),
        query_result("Organisation[33]{id: 915, name: \"Chang'an_Airlines\", type: \"company\", "
                      "url: \"http://dbpedia.org/resource/Chang'an_Airlines\"}"),
        query_result("::workAt[21]{workFrom: 2002}") });
    
    run_ldbc_iu_query_1(graph, rs);
    
    REQUIRE(rs == expected);
  }

  SECTION("query update #2") {
    result_set rs, expected;

    expected.append(
        {query_result("Person[0]{birthday: 628646400, browser: \"Firefox\", "
                      "creationDate: 1266161530, firstName: \"Mahinda\", "
                      "gender: \"male\", id: 933, lastName: \"Perera\", "
                      "locationIP: \"119.235.7.103\"}"),
         query_result("Post[19]{browser: \"Firefox\", content: \"About Alexander I of Russia, "
                        "lexander tried to introduce liberal reforms, while in the second half\", "
                        "creationDate: 1345456288, id: 3627, imageFile: \"\", language: \"uz\", "
                        "length: 98, locationIP: \"14.205.203.83\"}"),
         query_result("::LIKES[18]{creationDate: 1266161530}")});
    
    run_ldbc_iu_query_2(graph, rs);
    
    REQUIRE(rs == expected);
  }

  SECTION("query update #3") {
    result_set rs, expected;

    expected.append(
        {query_result("Person[10]{birthday: 467251200, browser: \"Chrome\", "
                      "creationDate: 1262412295, firstName: \"Emperor of Brazil\", "
                      "gender: \"female\", id: 1564, lastName: \"Silva\", "
                      "locationIP: \"192.223.88.63\"}"),
         query_result("Comment[25]{browser: \"Firefox\", content: \"Firefox|About Louis I of Hungary, "
                      "ittle lasting political results. Louis is theAbout Union of Sou\", "
                      "creationDate: 1326973191, id: 1642250, length: 89, "
                      "locationIP: \"85.154.120.237\"}"),
         query_result("::LIKES[18]{creationDate: 1327308990}")});
    
    run_ldbc_iu_query_3(graph, rs);
    
    REQUIRE(rs == expected);
  }

  SECTION("query update #4") {
    result_set rs, expected;

    expected.append(
        {query_result("Forum[34]{creationDate: 1262412305, id: 53975, title: "
                      "\"Wall of Emperor of Brazil Silva\"}"),
        query_result("Person[10]{birthday: 467251200, browser: \"Chrome\", "
                      "creationDate: 1262412295, firstName: \"Emperor of Brazil\", "
                      "gender: \"female\", id: 1564, lastName: \"Silva\", "
                      "locationIP: \"192.223.88.63\"}"), 
        query_result("::hasModerator[18]{}"),
        query_result("Tag[29]{id: 206, name: \"Charlemagne\", url: "
                      "\"http://dbpedia.org/resource/Charlemagne\"}"),
        query_result("::hasTag[19]{}") });
    
    run_ldbc_iu_query_4(graph, rs);
    
    REQUIRE(rs == expected);
  }

  SECTION("query update #5") {
    result_set rs, expected;

    expected.append(
        {query_result("Person[10]{birthday: 467251200, browser: \"Chrome\", "
                      "creationDate: 1262412295, firstName: \"Emperor of Brazil\", "
                      "gender: \"female\", id: 1564, lastName: \"Silva\", "
                      "locationIP: \"192.223.88.63\"}"),
         query_result("Forum[27]{creationDate: 1266194787, id: 37, title: \"Wall of Hồ Chí Do\"}"),
         query_result("::hasMember[18]{creationDate: 1266916225}")});
    
    run_ldbc_iu_query_5(graph, rs);
    
    REQUIRE(rs == expected);
  }

  SECTION("query update #6") {
    result_set rs, expected;

    expected.append(
        {query_result("Post[34]{browser: \"Safari\", content: \"About Alexander I of "
                        "Russia,  (23 December  1777 – 1 December  1825), (Russian: "
                        "Александр Благословенный, Aleksandr Blagoslovennyi, meaning Alexander the Bless\", "
                        "creationDate: 1315407147, id: 13439, imageFile: \"\", language: \"\"uz\"\", "
                        "length: 159, locationIP: \"46.19.159.176\"}"),
        query_result("Person[3]{birthday: 565315200, browser: \"Safari\", creationDate: 1282680826, "
                      "firstName: \"Fritz\", gender: \"female\", id: 65970697, lastName: \"Muller\", "
                      "locationIP: \"46.19.159.176\"}"),
        query_result("::hasCreator[18]{}"),
        query_result("Forum[28]{creationDate: 1285086335, id: 71489, "
                      "title: \"Group for Alexander_I_of_Russia in Umeå\"}"),
        query_result("::containerOf[19]{}"),
        query_result("Place[16]{id: 50, name: \"Germany\", type: \"country\", "
                      "url: \"http://dbpedia.org/resource/Germany\"}"),
        query_result("::isLocatedn[20]{}"),
        query_result("Tag[31]{id: 1679, name: \"Alexander_I_of_Russia\", "
                      "url: \"http://dbpedia.org/resource/Alexander_I_of_Russia\"}"),
        query_result("::hasTag[21]{}")});
    
    run_ldbc_iu_query_6(graph, rs);
    
    REQUIRE(rs == expected);
  }

  SECTION("query update #7") {
    result_set rs, expected;

    expected.append(
        {query_result("Comment[34]{browser: \"Chrome\", content: \"fine\", creationDate: 1326109755, "
                        "id: 442214, length: 4, locationIP: \"91.149.169.27\"}"),
        query_result("Person[11]{birthday: 631324800, browser: \"Chrome\", creationDate: 1293264475, "
                      "firstName: \"Ivan Ignatyevich\", gender: \"male\", id: 1043, lastName: \"Aleksandrov\", "
                      "locationIP: \"91.149.169.27\"}"),
        query_result("::hasCreator[18]{}"),
        query_result("Post[20]{browser: \"Internet Explorer\", content: \"About Louis I of Hungary, "
                      "dwig der Große, Bulgarian: Лудвиг I, Serbian: Лајош I Анжујски, Czech: Ludvík I. "
                      "Veliký, Li\", creationDate: 1326096328, id: 16492674, imageFile: \"\", "
                      "language: \"tk\", length: 117, locationIP: \"192.147.218.174\"}"),
        query_result("::replyOf[19]{}"),
        query_result("Place[17]{id: 63, name: \"Belarus\", type: \"country\", url: "
                      "\"http://dbpedia.org/resource/Belarus\"}"),
        query_result("::isLocatedn[20]{}"),
        query_result("Tag[31]{id: 1679, name: \"Alexander_I_of_Russia\", "
                      "url: \"http://dbpedia.org/resource/Alexander_I_of_Russia\"}"),
        query_result("::hasTag[21]{}")});
    
    run_ldbc_iu_query_7(graph, rs);
    
    REQUIRE(rs == expected);
  }

  SECTION("query update #8") {
    result_set rs, expected;

    expected.append(
        {query_result("Person[10]{birthday: 467251200, browser: \"Chrome\", "
                      "creationDate: 1262412295, firstName: \"Emperor of Brazil\", "
                      "gender: \"female\", id: 1564, lastName: \"Silva\", "
                      "locationIP: \"192.223.88.63\"}"),
        query_result("Person[7]{birthday: 592790400, browser: \"Internet Explorer\", "
                      "creationDate: 1266194777, firstName: \"Hồ Chí\", "
                      "gender: \"male\", id: 4194, lastName: \"Do\", "
                      "locationIP: \"103.2.223.188\"}"),
         query_result("::KNOWS[18]{creationDate: 1266916215}")});
    
    run_ldbc_iu_query_8(graph, rs);
    
    REQUIRE(rs == expected);
  }


#ifdef USE_TX
  graph->commit_transaction();
#endif

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}*/

/*TEST_CASE("Testing ALL LDBC interactive queries", "[ldbc]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto graph = create_graph1(pop);
  auto graph2 = create_graph2(pop);
#else
  auto graph = create_graph1();
  auto graph2 = create_graph2();
#endif

  run_all_ldbc_queries(graph, graph2); 

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph2); });
  pop.close();
  remove(test_path.c_str());
#endif
}*/


/**--------------------------------------------------------------------------------
 * ---------------(OLD) LDBC INTERACTIVE SHORT AND INSERT QUERIES -----------------
 * --------------------------------------------------------------------------------
 * --------------------------------------------------------------------------------
 * */

namespace pj = builtin;

void run_ldbc_is_query_1(graph_db_ptr &gdb, result_set &rs) {
  auto personId = 933;
  auto q = query(gdb)
               .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(personId); })
               .from_relationships(":isLocatedIn")
               .to_node("Place")
               .project({PExpr_(0, pj::string_property(res, "firstName")),
                         PExpr_(0, pj::string_property(res, "lastName")),
                         PExpr_(0, pj::int_to_datestring(
                                       pj::int_property(res, "birthday"))),
                         PExpr_(0, pj::string_property(res, "locationIP")),
                         PExpr_(0, pj::string_property(res, "browser")),
                         PExpr_(2, pj::int_property(res, "id")),
                         PExpr_(0, pj::string_property(res, "gender")),
                         PExpr_(0, pj::int_to_dtimestring(
                                       pj::int_property(res, "creationDate")))})
               .collect(rs);
  q.start();
  rs.wait();
}

void run_ldbc_is_query_2(graph_db_ptr &gdb, result_set &rs) {
  auto personId = 65;
  auto maxHops = 3; 

  auto q1 = query(gdb)
               .nodes_where("Person", "id",
                            [&](auto &c) { return c.equal(personId); })
               .to_relationships(":hasCreator")
               .limit(10)
               .from_node("Comment")
               .from_relationships({1, maxHops}, ":replyOf") 
               .to_node("Post")
               .from_relationships(":hasCreator")
               .to_node("Person")
               .project({PExpr_(2, pj::int_property(res, "id")),
                        PExpr_(2, pj::string_property(res, "content")),
                        PExpr_(2, pj::int_to_dtimestring(
                                       pj::int_property(res, "creationDate"))),
                        PExpr_(4, pj::int_property(res, "id")),
                        PExpr_(6, pj::int_property(res, "id")),
                        PExpr_(6, pj::string_property(res, "firstName")),
                        PExpr_(6, pj::string_property(res, "lastName")) })
               .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                          auto t1 = pj::dtimestring_to_int(boost::get<std::string>(qr1[2]));
                          auto t2 = pj::dtimestring_to_int(boost::get<std::string>(qr2[2]));
                          if(t1 == t2)
                              return boost::get<int>(qr1[0]) > boost::get<int>(qr2[0]);
                          return t1 > t2; })
               .collect(rs);

  auto q2 = query(gdb)
               .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(personId); })
               .to_relationships(":hasCreator")
               .limit(10)
               .from_node("Post")
               .project({PExpr_(2, pj::int_property(res, "id")),
                        PExpr_(2, !pj::string_property(res, "content").empty() ? 
                            pj::string_property(res, "content") : pj::string_property(res, "imageFile")),
                        PExpr_(2, pj::int_to_dtimestring(
                                       pj::int_property(res, "creationDate"))),
                        PExpr_(2, pj::int_property(res, "id")),
                        PExpr_(0, pj::int_property(res, "id")),
                        PExpr_(0, pj::string_property(res, "firstName")),
                        PExpr_(0, pj::string_property(res, "lastName")) })
               .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                          auto t1 = pj::dtimestring_to_int(boost::get<std::string>(qr1[2]));
                          auto t2 = pj::dtimestring_to_int(boost::get<std::string>(qr2[2]));
                          if(t1 == t2)
                              return boost::get<int>(qr1[0]) > boost::get<int>(qr2[0]);
                          return t1 > t2; })
               .collect(rs);

  query::start({&q2, &q1});
  rs.wait();
}

void run_ldbc_is_query_3(graph_db_ptr &gdb, result_set &rs) {
	auto personId = 933;

  auto q = query(gdb)
                .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(personId); })
                .from_relationships(":KNOWS")
                .to_node("Person")
                .project({PExpr_(2, pj::int_property(res, "id")),
                          PExpr_(2, pj::string_property(res, "firstName")),
                          PExpr_(2, pj::string_property(res, "lastName")),
                          PExpr_(1, pj::int_to_dtimestring(pj::int_property(res, "creationDate"))) 
                          })
                .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                          auto t1 = pj::dtimestring_to_int(boost::get<std::string>(qr1[3]));
                          auto t2 = pj::dtimestring_to_int(boost::get<std::string>(qr2[3]));
                          if(t1 == t2)
                              return boost::get<int>(qr1[0]) < boost::get<int>(qr2[0]);
                          return t1 > t2; })
                .collect(rs);
  
  q.start();
  rs.wait();
}

void run_ldbc_is_query_4(graph_db_ptr &gdb, result_set &rs) {
	auto postId = 13743895;

	auto q = query(gdb)
                .nodes_where("Post", "id",
                              [&](auto &p) { return p.equal(postId); })
                .project({PExpr_(0, pj::int_to_dtimestring(pj::int_property(res, "creationDate"))),
                          PExpr_(0, !pj::string_property(res, "content").empty() ? 
                            pj::string_property(res, "content") : pj::string_property(res, "imageFile")) })
                .collect(rs);
				
	q.start();
	rs.wait();
}

void run_ldbc_is_query_5(graph_db_ptr &gdb, result_set &rs) {
	auto commentId = 12362343;

	auto q = query(gdb)
                .nodes_where("Comment", "id",
                              [&](auto &c) { return c.equal(commentId); })
                .from_relationships(":hasCreator")
                .to_node("Person")
                .project({PExpr_(2, pj::int_property(res, "id")),
                          PExpr_(2, pj::string_property(res, "firstName")),
                          PExpr_(2, pj::string_property(res, "lastName")) })
                .collect(rs);
	q.start();
	rs.wait();
}

void run_ldbc_is_query_6(graph_db_ptr &gdb, result_set &rs) {
  auto commentId = 16492677;
  auto postID = 16492674;
  auto maxHops = 3;
    
  auto q1 = query(gdb)
                .nodes_where("Post", "id",
                  [&](auto &p) { return p.equal(postID); })
                .to_relationships(":containerOf")
                .from_node("Forum")
                .from_relationships(":hasModerator")
                .to_node("Person")
                .project({PExpr_(2, pj::int_property(res, "id")),
                          PExpr_(2, pj::string_property(res, "title")),
                          PExpr_(4, pj::int_property(res, "id")),
                          PExpr_(4, pj::string_property(res, "firstName")),
                          PExpr_(4, pj::string_property(res, "lastName")) })
                .collect(rs);
  
  auto q2 = query(gdb)
                .nodes_where("Post", "id",
                  [&](auto &c) { return c.equal(commentId); })
                .from_relationships({1, maxHops}, ":replyOf") 
                .to_node("Post")
                .to_relationships(":containerOf")
                .from_node("Forum")
                .from_relationships(":hasModerator")
                .to_node("Person")
                .project({PExpr_(4, pj::int_property(res, "id")),
                          PExpr_(4, pj::string_property(res, "title")),
                          PExpr_(6, pj::int_property(res, "id")),
                          PExpr_(6, pj::string_property(res, "firstName")),
                          PExpr_(6, pj::string_property(res, "lastName")) })
                .collect(rs);
	
	query::start({&q1, &q2});
	rs.wait(); 
}

void run_ldbc_is_query_7(graph_db_ptr &gdb, result_set &rs) {
    auto commentId = 16492676;
     
    auto q1 = query(gdb)
                  .nodes_where("Comment", "id",
                  	[&](auto &c) { return c.equal(commentId); })
                  .from_relationships(":hasCreator")
				          .to_node("Person");
    
    auto q2 = query(gdb)
                  .nodes_where("Comment", "id",
                  	[&](auto &c) { return c.equal(commentId); })
                  .to_relationships(":replyOf")    
                  .from_node("Comment")
				          .from_relationships(":hasCreator")
				          .to_node("Person")
                  .outerjoin({4, 2}, q1)
				          .project({PExpr_(2, pj::int_property(res, "id")),
                            PExpr_(2, pj::string_property(res, "content")),
                            PExpr_(2, pj::int_to_dtimestring(pj::int_property(res, "creationDate"))),
                            PExpr_(4, pj::int_property(res, "id")),
                            PExpr_(4, pj::string_property(res, "firstName")),
                            PExpr_(4, pj::string_property(res, "lastName")),
                            PExpr_(8, res.type() == typeid(rship_description) ?
                                        std::string("true") : std::string("false")) })
                  .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                          auto t1 = pj::dtimestring_to_int(boost::get<std::string>(qr1[2]));
                          auto t2 = pj::dtimestring_to_int(boost::get<std::string>(qr2[2]));
                          if(t1 == t2)
                              return boost::get<int>(qr1[3]) < boost::get<int>(qr2[3]);
                          return t1 > t2; })
                  .collect(rs);

	query::start({&q1, &q2});
	rs.wait();
}

void run_ldbc_iu_query_1(graph_db_ptr &gdb, result_set &rs) {
  auto personId = 2370;
  auto fName = std::string("Yang");
  auto lName = std::string("Zhu");
  auto gender = std::string("male"); 
  auto birthday = pj::datestring_to_int("1982-02-21");
  auto creationDate = pj::dtimestring_to_int("2010-01-17 07:59:27.746");
  auto locationIP = std::string("1.183.127.173"); 
  auto browser = std::string("Internet Explorer");
  auto cityId = 505;
  auto language = std::string("\"zh\", \"en\""); 
  auto email = std::string("\"Yang2370@gmail.com\", \"Yang2370@hotmail.com\"");  
  auto tagId = 61;
  auto uniId = 2213;
  auto classYear = 2001;
  auto companyId = 915;
  auto workFrom = 2002;

  auto q1 = query(gdb).nodes_where("Place", "id",
                                   [&](auto &c) { return c.equal(cityId); });

  auto q2 = query(gdb).nodes_where("Tag", "id",
                                   [&](auto &t) { return t.equal(tagId); });

  auto q3 = query(gdb).nodes_where("Organisation", "id",
                                   [&](auto &u) { return u.equal(uniId); });

  auto q4 = query(gdb).nodes_where("Organisation", "id",
                                   [&](auto &c) { return c.equal(companyId); });

  auto q5 = query(gdb).create("Person",
                              {{"id", boost::any(personId)},
                              {"firstName", boost::any(fName)},
                              {"lastName", boost::any(lName)},
                              {"gender", boost::any(gender)},
                              {"birthday", boost::any(birthday)},
                              {"creationDate", boost::any(creationDate)},
                              {"locationIP", boost::any(locationIP)},
                              {"browser", boost::any(browser)},
                              {"language", boost::any(language)},
                              {"email", boost::any(email)}})
                      .crossjoin(q1)
                      .create_rship({0, 1}, ":isLocatedIn", {})
                      .crossjoin(q2)
                      .create_rship({0, 3}, ":hasInterest", {})
                      .crossjoin(q3)
                      .create_rship({0, 5}, ":studyAt", {{"classYear", boost::any(classYear)}})
                      .crossjoin(q4)
                      .create_rship({0, 7}, ":workAt", {{"workFrom", boost::any(workFrom)}})
                      .collect(rs);

  query::start({&q1, &q2, &q3, &q4, &q5});
}

void run_ldbc_iu_query_2(graph_db_ptr &gdb, result_set &rs) {
  auto personId = 933;
  auto postId = 3627; 
  auto creationDate = pj::dtimestring_to_int("2010-02-14 15:32:10.447");

  auto q1 = query(gdb).nodes_where("Post", "id",
                                   [&](auto &p) { return p.equal(postId); });

  auto q2 =
      query(gdb)
          .nodes_where("Person", "id",
                       [&](auto &p) { return p.equal(personId); })
          .crossjoin(q1)
          .create_rship({0, 1}, ":LIKES", {{"creationDate", boost::any(creationDate)}})
          .collect(rs);

  query::start({&q1, &q2});
}

void run_ldbc_iu_query_3(graph_db_ptr &gdb, result_set &rs) {
  auto personId = 1564;
  auto commentId = 1642250;
  auto creationDate = pj::dtimestring_to_int("2012-01-23 08:56:30.617");

  auto q1 = query(gdb).nodes_where("Comment", "id",
                                   [&](auto &c) { return c.equal(commentId); });
  auto q2 =
      query(gdb)
          .nodes_where("Person", "id",
                       [&](auto &p) { return p.equal(personId); })
          .crossjoin(q1)
          .create_rship({0, 1}, ":LIKES", {{"creationDate", boost::any(creationDate)}})
          .collect(rs);

  query::start({&q1, &q2});
}

void run_ldbc_iu_query_4(graph_db_ptr &gdb, result_set &rs) {
  auto personId = 1564;
  auto tagId = 206;
  auto forumId = 53975;
  auto title = std::string("Wall of Emperor of Brazil Silva");
  auto creationDate = pj::dtimestring_to_int("2010-01-02 06:05:05.320");

  auto q1 = query(gdb).nodes_where("Person", "id",
                                   [&](auto &p) { return p.equal(personId); });

  auto q2 = query(gdb).nodes_where("Tag", "id",
                                   [&](auto &t) { return t.equal(tagId); });

  auto q3 = query(gdb).create("Forum",
                              {{"id", boost::any(forumId)},
                              {"title", boost::any(title)},
                              {"creationDate", boost::any(creationDate)} })
                      .crossjoin(q1)
                      .create_rship({0, 1}, ":hasModerator", {})
                      .crossjoin(q2)
                      .create_rship({0, 3}, ":hasTag", {})
                      .collect(rs);

  query::start({&q1, &q2, &q3});
}

void run_ldbc_iu_query_5(graph_db_ptr &gdb, result_set &rs) {
  auto personId = 1564;
  auto forumId = 37;
  auto joinDate = pj::dtimestring_to_int("2010-02-23 09:10:25.466");

  auto q1 = query(gdb).nodes_where("Forum", "id",
                                   [&](auto &f) { return f.equal(forumId); });
  auto q2 =
      query(gdb)
          .nodes_where("Person", "id",
                       [&](auto &p) { return p.equal(personId); })
          .crossjoin(q1)
          .create_rship({0, 1}, ":hasMember", {{"creationDate", boost::any(joinDate)}})
          .collect(rs);

  query::start({&q1, &q2});
}

void run_ldbc_iu_query_6(graph_db_ptr &gdb, result_set &rs) {
  auto postId = 13439;  
  auto imageFile = std::string("");
  auto creationDate = pj::dtimestring_to_int("2011-09-07 14:52:27.809");
  auto locationIP = std::string("46.19.159.176"); 
  auto browser = std::string("Safari");
  auto language = std::string("\"uz\""); 
  auto content = std::string("About Alexander I of Russia,  (23 December  1777 – 1 December  1825), (Russian: "
                            "Александр Благословенный, Aleksandr Blagoslovennyi, meaning Alexander the Bless"); 
  auto length = 159;
  auto personId = 65970697;
  auto forumId = 71489; 
  auto countryId = 50;
  auto tagId = 1679;

  auto q1 = query(gdb).nodes_where("Person", "id",
                                   [&](auto &p) { return p.equal(personId); });

  auto q2 = query(gdb).nodes_where("Forum", "id",
                                   [&](auto &f) { return f.equal(forumId); });

  auto q3 = query(gdb).nodes_where("Place", "id",
                                   [&](auto &c) { return c.equal(countryId); });

  auto q4 = query(gdb).nodes_where("Tag", "id",
                                   [&](auto &t) { return t.equal(tagId); });

  auto q5 = query(gdb).create("Post",
                            {{"id", boost::any(postId)}, 
                              {"imageFile", boost::any(imageFile)},
                              {"creationDate", boost::any(creationDate)},
                              {"locationIP", boost::any(locationIP)},
                              {"browser", boost::any(browser)},
                              {"language", boost::any(language)}, 
                              {"content", boost::any(content)},
                              {"length", boost::any(length)} })
                      .crossjoin(q1)
                      .create_rship({0, 1}, ":hasCreator", {})
                      .crossjoin(q2)
                      .create_rship({3, 0}, ":containerOf", {})
                      .crossjoin(q3)
                      .create_rship({0, 5}, ":isLocatedn", {})
                      .crossjoin(q4)
                      .create_rship({0, 7}, ":hasTag", {})
                      .collect(rs);

  query::start({&q1, &q2, &q3, &q4, &q5});
}

void run_ldbc_iu_query_7(graph_db_ptr &gdb, result_set &rs) {
  auto commentId = 442214; 
  auto creationDate = pj::dtimestring_to_int("2012-01-09 11:49:15.991");
  auto locationIP = std::string("91.149.169.27"); 
  auto browser = std::string("Chrome");
  auto content = std::string("fine"); 
  auto length = 4;
  auto personId = 1043;
  auto postId = 16492674; 
  auto countryId = 63;
  auto tagId = 1679;

  auto q1 = query(gdb).nodes_where("Person", "id",
                                   [&](auto &p) { return p.equal(personId); });

  auto q2 = query(gdb).nodes_where("Post", "id",
                                   [&](auto &f) { return f.equal(postId); });

  auto q3 = query(gdb).nodes_where("Place", "id",
                                   [&](auto &c) { return c.equal(countryId); });

  auto q4 = query(gdb).nodes_where("Tag", "id",
                                   [&](auto &t) { return t.equal(tagId); });

  auto q5 = query(gdb).create("Comment",
                              {{"id", boost::any(commentId)},
                              {"creationDate", boost::any(creationDate)},
                              {"locationIP", boost::any(locationIP)},
                              {"browser", boost::any(browser)},
                              {"content", boost::any(content)},
                              {"length", boost::any(length)} })
                      .crossjoin(q1)
                      .create_rship({0, 1}, ":hasCreator", {})
                      .crossjoin(q2)
                      .create_rship({0, 3}, ":replyOf", {})
                      .crossjoin(q3)
                      .create_rship({0, 5}, ":isLocatedn", {})
                      .crossjoin(q4)
                      .create_rship({0, 7}, ":hasTag", {})
                      .collect(rs);

  query::start({&q1, &q2, &q3, &q4, &q5});
}

void run_ldbc_iu_query_8(graph_db_ptr &gdb, result_set &rs) {
  auto personId_1 = 1564;
  auto personId_2 = 4194;
  auto creationDate = pj::dtimestring_to_int("2010-02-23 09:10:15.466");

  auto q1 = query(gdb).nodes_where("Person", "id",
                                   [&](auto &p) { return p.equal(personId_2); });
  auto q2 =
      query(gdb)
          .nodes_where("Person", "id",
                       [&](auto &p) { return p.equal(personId_1); })
          .crossjoin(q1)
          .create_rship({0, 1}, ":KNOWS", {{"creationDate", boost::any(creationDate)}})
          .collect(rs);

  query::start({&q1, &q2});
}

void run_all_ldbc_queries(graph_db_ptr &gdb, graph_db_ptr &gdb2) {
  // the query set
  std::function<void(graph_db_ptr &, result_set &)> query_set[] = {
      run_ldbc_is_query_1, run_ldbc_is_query_2, run_ldbc_is_query_3, 
      run_ldbc_is_query_4, run_ldbc_is_query_5, run_ldbc_is_query_6, run_ldbc_is_query_7,
      run_ldbc_iu_query_1, run_ldbc_iu_query_2, run_ldbc_iu_query_3, run_ldbc_iu_query_4,
      run_ldbc_iu_query_5, run_ldbc_iu_query_6, run_ldbc_iu_query_7, run_ldbc_iu_query_8};
    
  std::size_t qnum = 1;

  // for each query we measure the time and run it in a transaction
  for (auto f : query_set) {
    result_set rs;
    auto start_qp = std::chrono::steady_clock::now();

    auto tx = gdb->begin_transaction();
    if (qnum == 2)
      f(gdb2, rs);
    else
      f(gdb, rs);
    gdb->commit_transaction();

    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "Query #" << qnum++ << " executed in "
              << std::chrono::duration_cast<std::chrono::microseconds>(end_qp -
                                                                       start_qp)
                     .count()
              << " μs" << std::endl;
  }
}


/**---------------------------------------------------
 * ------------------ SNB ----------------------------
 * ---------------------------------------------------
 * ----------------------------------------------------
 * */

const std::string snb_sta("/home/data/SNB_SF_1/static/");
const std::string snb_dyn("/home/data/SNB_SF_1/dynamic/");

void load_snb_data(graph_db_ptr &graph, 
                    std::vector<std::string> &node_files,
                    std::vector<std::string> &rship_files){
  auto delim = '|';
  graph_db::mapping_t mapping;
  bool nodes_imported = false, rships_imported = false;
  
  if (!node_files.empty()){
    std::cout << "\n######## \n# NODES \n######## \n";

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
      std::cout << num_nodes[i] << " \"" << label << "\" node objects imported \n";
      if (num_nodes[i] > 0)
        nodes_imported = true;
      i++;
    }
  }

  if (!rship_files.empty()){
    std::cout << "\n \n################ \n# RELATIONSHIPS \n################ \n";
    
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
      std::cout << num_rships[i] << " (" << fn[0] << ")-[\"" << label << "\"]->(" 
                                  << fn[2] << ") relationship objects imported \n";
      if (num_rships[i] > 0)
        rships_imported = true;
      i++;
    }
  }
  assert(nodes_imported || rships_imported); // data imported to run benchmark 
}

graph_db_ptr create_graph(
#ifdef USE_PMDK
    nvm::pool_base &pop
#endif
) {
#ifdef USE_PMDK
  graph_db_ptr graph;
  nvm::transaction::run(pop, [&] { graph = p_make_ptr<graph_db>(); });
#else
  auto graph = p_make_ptr<graph_db>();
#endif
  return graph;
}

/*TEST_CASE("ldbc_is_queries", "[ldbc]") {
#ifdef USE_PMDK
  if (access(test_path.c_str(), F_OK) == 0)
    remove(test_path.c_str());
  auto pop = prepare_pool();
  auto graph = create_graph(pop);
#else
  auto graph = create_graph();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  std::vector<std::string> node_files = {snb_sta + "place_0_0.csv", snb_sta + "organisation_0_0.csv",
                                         snb_sta + "tagclass_0_0.csv", snb_sta + "tag_0_0.csv",
                                         snb_dyn + "comment_0_0.csv", snb_dyn + "forum_0_0.csv",
                                         snb_dyn + "person_0_0.csv", snb_dyn + "post_0_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "comment_hasCreator_person_0_0.csv",
                                          snb_dyn + "comment_isLocatedIn_place_0_0.csv",
                                          snb_dyn + "comment_replyOf_comment_0_0.csv",
                                          snb_dyn + "comment_replyOf_post_0_0.csv",
                                          snb_dyn + "forum_containerOf_post_0_0.csv",
                                          snb_dyn + "forum_hasMember_person_0_0.csv",
                                          snb_dyn + "forum_hasModerator_person_0_0.csv",
                                          snb_dyn + "forum_hasTag_tag_0_0.csv",
                                          snb_dyn + "person_hasInterest_tag_0_0.csv",
                                          snb_dyn + "person_isLocatedIn_place_0_0.csv",
                                          snb_dyn + "person_knows_person_0_0.csv",
                                          snb_dyn + "person_likes_comment_0_0.csv",
                                          snb_dyn + "person_likes_post_0_0.csv",
                                          snb_dyn + "post_hasCreator_person_0_0.csv",
                                          snb_dyn + "comment_hasTag_tag_0_0.csv",
                                          snb_dyn + "post_hasTag_tag_0_0.csv",
                                          snb_dyn + "post_isLocatedIn_place_0_0.csv",
                                          snb_dyn + "person_studyAt_organisation_0_0.csv",
                                          snb_dyn + "person_workAt_organisation_0_0.csv"};

  std::vector<uint64_t> personIds_is_1 = {933, 24189255812290, 6597069773744, 2199023266220, 13194139544176,
                                      17592186050570, 24189255815734, 28587302330379, 32985348842922, 3601,
                                      4398046511870, 32985348834284, 17592186045096, 17592186053245, 4398046520495,
                                      4233, 344, 10995116286457, 10976, 24189255813927};

  std::vector<uint64_t> personIds_is_2 = {65, 28587302330379, 3601, 24189255817217, 4398046511870,
                                      8698, 6597069773744, 13194139544176, 17592186050570, 6597069766993,
                                      24189255815734, 26388279077330, 15393162799262, 32985348843825, 32985348843760,
                                      32985348842653, 13194139540894, 13194139540856, 8796093028361, 6597069766998};  

  std::vector<uint64_t> personIds_is_3 = {19791209304051, 28587302326940, 2199023262021, 8796093027111, 2199023262994,
                                    6597069773744, 13194139544176, 17592186050570, 30786325588658, 24189255815734,
                                    6597069774931, 13194139544258, 15393162791382, 21990232558836, 28587302322686,
                                    24189255820923, 32985348833548, 30786325581208, 26388279074032, 32985348834375};
  
  std::vector<uint64_t> PostIds_is_4 = {1374389534801, 687194926510, 1236950581577, 824633724379, 687194903818,
                                      549755930326, 1649267546616, 1649267453265, 1924145376549, 1099511719169};

  std::vector<uint64_t> CommentIds_is_4 = {1236950581249, 1374389535139, 687194767797, 962072674365, 274877974096,
                                      1374389620660, 1374389535186, 2061584302604, 1099511678319, 1099511755889};
  
  std::vector<uint64_t> PostIds_is_5 = {1649267611029, 1649267641500, 1649267717129, 549756117312, 962073027971,
                                      1924145709571, 1786706759766, 137439322338, 962073047211, 1786706792809};

  std::vector<uint64_t> commentIds_is_5 = {2061584429975, 1099511764068, 1511828638961, 1099511794459, 1924145529653,
                                      137439153914, 1374389758562, 687194998602, 1099511869402, 1649267722310};
  
  std::vector<uint64_t> PostIds_is_6 = {1374389534795, 3, 246, 1786710746552, 1786710746860,
                                      962077492609, 4818574, 137443772206, 4818783, 1649273779906,
                                      1099512706784, 1924145709571, 274879100510, 2061585683162, 2061585683383,
                                      824638318943, 962073868902, 962076990540, 1236955780271, 1924151699930};
  std::vector<uint64_t> commentIds_is_6 = {549756150652, 2061587049723, 1786710610862, 1924150141935, 1649271672251,
                                      1099518023455, 1511835112930, 962079298952, 549762439424, 1786707596571,
                                      824635086444, 2199024637100, 549762296256, 412319368884, 1924148311956,
                                      687196868319, 1786710956334, 2882812, 274878321446, 687194840176};
  
  std::vector<uint64_t> commentIds_is_7 = {549755814584, 962074006383, 1374390866272, 1511833539098, 687196097161,
                                      1786710956803, 1924148155034, 824636527214, 2061587107320, 274880712071,
                                      962075482675, 1786708701848, 2061588922925, 4784850, 4784913,
                                      412321645469, 1374394320184, 1374390902281, 1511834991008, 824634964783};
  
  load_snb_data(graph, node_files, rship_files);
  std::cout << "\n\n";

  auto i = 0;
  //result_set rs_is_1[20];
  for (auto id : personIds_is_1){
    auto start_qp = std::chrono::steady_clock::now();
    //ldbc_is_query_1(graph, rs_is_1[i++], id);
    ldbc_is_query_1(graph, id);
    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "ldbc_is_query_1 with PersonId " << id << " executed in "
                << std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp).count()
                << " μs" << std::endl;
  }
  std::cout << "\n\n";

  i = 0;
  //result_set rs_is_2[20];
  for (auto id : personIds_is_2){
    auto start_qp = std::chrono::steady_clock::now();
    //ldbc_is_query_2_p(graph, rs_is_2[i++], id);
    ldbc_is_query_2_p(graph, id);
    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "ldbc_is_query_2_p with PersonId " << id << " executed in "
                << std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp).count()
                << " μs" << std::endl;
  }
  std::cout << "\n\n";

  i = 0;
  //result_set rs_is_2[20];
  for (auto id : personIds_is_2){
    auto start_qp = std::chrono::steady_clock::now();
    //ldbc_is_query_2_c(graph, rs_is_2[i++], id);
    ldbc_is_query_2_c(graph, id);
    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "ldbc_is_query_2_c with PersonId " << id << " executed in "
                << std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp).count()
                << " μs" << std::endl;
  }
  std::cout << "\n\n";

  i = 0;
  //result_set rs_is_3[20];
  for (auto id : personIds_is_3){
    auto start_qp = std::chrono::steady_clock::now();
    //ldbc_is_query_3(graph, rs_is_3[i++], id);
    ldbc_is_query_3(graph, id);
    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "ldbc_is_query_3 with PersonId " << id << " executed in "
                << std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp).count()
                << " μs" << std::endl;
  }
  std::cout << "\n\n";

  i = 0;
  //result_set rs_is_4[10];
  for (auto id : PostIds_is_4){
    auto start_qp = std::chrono::steady_clock::now();
    //ldbc_is_query_4(graph, rs_is_4[i++], id);
    ldbc_is_query_4_p(graph, id);
    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "ldbc_is_query_4_p with PostId " << id << " executed in "
                << std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp).count()
                << " μs" << std::endl;
  }
  std::cout << "\n\n";

  i = 0;
  //result_set rs_is_4[10];
  for (auto id : CommentIds_is_4){
    auto start_qp = std::chrono::steady_clock::now();
    //ldbc_is_query_4(graph, rs_is_4[i++], id);
    ldbc_is_query_4_c(graph, id);
    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "ldbc_is_query_4_c with CommentId " << id << " executed in "
                << std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp).count()
                << " μs" << std::endl;
  }
  std::cout << "\n\n";

  i = 0;
  //result_set rs_is_5[10];
  for (auto id : PostIds_is_5){
    auto start_qp = std::chrono::steady_clock::now();
    //ldbc_is_query_5(graph, rs_is_5[i++], id);
    ldbc_is_query_5_p(graph, id);
    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "ldbc_is_query_5_p with PostId " << id << " executed in "
                << std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp).count()
                << " μs" << std::endl;
  }
  std::cout << "\n\n";

  i = 0;
  //result_set rs_is_5[10];
  for (auto id : commentIds_is_5){
    auto start_qp = std::chrono::steady_clock::now();
    //ldbc_is_query_5(graph, rs_is_5[i++], id);
    ldbc_is_query_5_c(graph, id);
    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "ldbc_is_query_5_c with commentId " << id << " executed in "
                << std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp).count()
                << " μs" << std::endl;
  }
  std::cout << "\n\n";

  i = 0;
  //result_set rs_is_6[20];  
  for (auto id : PostIds_is_6){
    auto start_qp = std::chrono::steady_clock::now();
    //ldbc_is_query_6(graph, rs_is_6[i], id);
    ldbc_is_query_6_p(graph, id);
    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "ldbc_is_query_6_p with PostId " << id << " executed in "
                << std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp).count()
                << " μs" << std::endl;
    i++;
  }
  std::cout << "\n\n";

  i = 0;
  //result_set rs_is_6[20];  
  for (auto id : commentIds_is_6){
    auto start_qp = std::chrono::steady_clock::now();
    //ldbc_is_query_6(graph, rs_is_6[i], id);
    ldbc_is_query_6_c(graph, id);
    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "ldbc_is_query_6_c with commentId " << id << " executed in "
                << std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp).count()
                << " μs" << std::endl;
    i++;
  }
  std::cout << "\n\n";

  i = 0;
  //result_set rs_is_7[20];
  for (auto id : commentIds_is_7){
    auto start_qp = std::chrono::steady_clock::now();
    //ldbc_is_query_7(graph, rs_is_7[i], id);
    ldbc_is_query_7(graph, id);
    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "ldbc_is_query_7 with commentId " << id << " executed in "
                << std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp).count()
                << " μs" << std::endl;
    i++;
  }
  std::cout << "\n\n";

#ifdef USE_TX
  graph->commit_transaction();
#endif

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}*/


TEST_CASE("ldbc_iu_queries", "[ldbc]") {
#ifdef USE_PMDK
  if (access(test_path.c_str(), F_OK) == 0)
    remove(test_path.c_str());
  auto pop = prepare_pool();
  auto graph = create_graph(pop);
#else
  auto graph = create_graph();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  std::vector<std::string> node_files = {snb_sta + "place_0_0.csv", snb_sta + "organisation_0_0.csv",
                                         snb_sta + "tagclass_0_0.csv", snb_sta + "tag_0_0.csv",
                                         snb_dyn + "comment_0_0.csv", snb_dyn + "forum_0_0.csv",
                                         snb_dyn + "person_0_0.csv", snb_dyn + "post_0_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "comment_hasCreator_person_0_0.csv",
                                          snb_dyn + "comment_isLocatedIn_place_0_0.csv",
                                          snb_dyn + "comment_replyOf_comment_0_0.csv",
                                          snb_dyn + "comment_replyOf_post_0_0.csv",
                                          snb_dyn + "forum_containerOf_post_0_0.csv",
                                          snb_dyn + "forum_hasMember_person_0_0.csv",
                                          snb_dyn + "forum_hasModerator_person_0_0.csv",
                                          snb_dyn + "forum_hasTag_tag_0_0.csv",
                                          snb_dyn + "person_hasInterest_tag_0_0.csv",
                                          snb_dyn + "person_isLocatedIn_place_0_0.csv",
                                          snb_dyn + "person_knows_person_0_0.csv",
                                          snb_dyn + "person_likes_comment_0_0.csv",
                                          snb_dyn + "person_likes_post_0_0.csv",
                                          snb_dyn + "post_hasCreator_person_0_0.csv",
                                          snb_dyn + "comment_hasTag_tag_0_0.csv",
                                          snb_dyn + "post_hasTag_tag_0_0.csv",
                                          snb_dyn + "post_isLocatedIn_place_0_0.csv",
                                          snb_dyn + "person_studyAt_organisation_0_0.csv",
                                          snb_dyn + "person_workAt_organisation_0_0.csv"};

  std::vector<std::vector<uint64_t>> uint64_props_iu_1 = {
    {9999999990999, 505, 61, 7954, 5},
    {9999999999999, 990, 16065, 3179, 5045},
    {9999999900999, 1288, 15835, 3049, 5753},
    {999439900999, 1288, 186, 3111, 6666},
    {9999999449999, 1350, 2515, 941, 2328},
    {888888999999, 1262, 40, 4, 7939},
    {9989299900999, 337, 9764, 3714, 10},
    {912345989999, 412, 14086, 6154, 7216},
    {7659966990999, 480, 15834, 4198, 6933},
    {97999999999998, 526, 3724, 1327, 2314},
    {8765345665433, 569, 9406, 4813, 6102},
    {90843284933333, 610, 61, 1437, 7954},
    {9999999990999, 760, 5479, 7731, 307},
    {6383865350979, 817, 3751, 1380, 676},
    {1356785432345, 857, 1409, 4030, 1211},
    {56543245674567, 916, 6042, 3023, 4719},
    {9876543234567, 967, 389, 6265, 4344},
    {8473462543452, 1039, 3540, 6947, 836},
    {9997654456999, 1132, 3456, 4034, 5364},
    {99999989999999, 1146, 1251, 379, 0}
  };

  std::vector<std::vector<std::string>> str_props_iu_1 = {
    {"firstname1", "lastname1", "male", "1982-4-27", "2010-02-14T01:51:21.746+0000", "1.183.127.173", "chrome", "tamil", "ar@xyz.com"},
    {"firstname2", "lastname2", "male", "1997-4-27", "2010-11-14T01:51:21.746+0000", "13.183.167.173", "safari", "deutsch", "asASA@SSSS.com"},
    {"firstname3", "lastname3", "female", "1975-12-10", "2014-01-15T01:51:21.746+0000", "33.13.128.183", "explorer", "english", "sss@wwww.com"},
    {"firstname4", "lastname4", "female", "1974-12-10", "2014-01-15T01:51:21.746+0000", "33.13.128.183", "explorer", "english", "sss@wwww.com"},
    {"firstname5", "lastname5", "female", "1982-4-19", "2003-02-14T01:51:21.746+0000", "1.183.127.173", "chrome", "kannada", "ar@xyz.com"},
    {"firstname6", "lastname6", "female", "1997-3-27", "2005-11-14T01:51:21.746+0000", "13.183.167.173", "safari", "deutsch", "asASA@SSSS.com"},
    {"firstname7", "lastname7", "female", "1975-12-10", "2008-01-15T01:51:21.746+0000", "33.13.128.183", "explorer", "english", "sss@wwww.com"},
    {"firstname8", "lastname8", "female", "1996-4-17", "2009-05-14T01:51:21.746+0000", "10.183.117.123", "firefox", "urdu", "uuuu@ztrzrzz.com"},
    {"firstname9", "lastname9", "male", "2000-11-01", "2011-02-14T01:51:21.746+0000", "1.183.127.173", "chrome", "tamil", "ar@xyz.com"},
    {"firstname10", "lastname10", "male", "2003-4-27", "2007-11-14T01:51:21.746+0000", "13.183.167.173", "safari", "deutsch", "asASA@SSSS.com"},
    {"firstname11", "lastname11", "female", "2002-12-14", "2005-01-15T01:51:21.746+0000", "33.13.128.183", "explorer", "english", "sss@wwww.com"},
    {"firstname12", "lastname12", "male", "2012-01-15", "2008-05-14T01:51:21.746+0000", "10.183.117.123", "firefox", "arabic", "uuuu@ztrzrzz.com"},
    {"firstname13", "lastname13", "female", "2005-10-29", "2010-02-14T01:51:21.746+0000", "1.183.127.173", "chrome", "french", "ar@xyz.com"},
    {"firstname14", "lastname14", "male", "1993-02-30", "2010-11-14T01:51:21.746+0000", "13.183.167.173", "safari", "deutsch", "asASA@SSSS.com"},
    {"firstname15", "lastname15", "female", "1997-12-31", "2014-01-15T01:51:21.746+0000", "33.13.128.183", "explorer", "english", "sss@wwww.com"},
    {"firstname16", "lastname16", "female", "1989-05-16", "2010-05-14T01:51:21.746+0000", "10.183.117.123", "firefox", "persian", "uuuu@ztrzrzz.com"},
    {"firstname17", "lastname17", "male", "1988-05-11", "2010-02-14T01:51:21.746+0000", "1.183.127.173", "chrome", "tamil", "ar@xyz.com"},
    {"firstname18", "lastname18", "female", "1999-07-27", "2010-11-14T01:51:21.746+0000", "13.183.167.173", "safari", "deutsch", "asASA@SSSS.com"},
    {"firstname19", "lastname19", "female", "1975-10-10", "2014-01-15T01:51:21.746+0000", "33.13.128.183", "explorer", "english", "sss@wwww.com"},
    {"firstname20", "lastname20", "female", "1982-01-17", "2010-05-12T01:51:21.746+0000", "10.183.117.123", "firefox", "spanish", "uuuu@ztrzrzz.com"}
  };

  std::vector<std::vector<int>> int_props_iu_1 = {
    {2001, 2012},
    {1988, 1999},
    {2000, 2010},
    {2000, 2010},
    {1987, 1999},
    {2001, 2015},
    {1979, 1997},
    {1977, 1996},
    {2005, 2016},
    {2009, 2019},
    {2011, 2020},
    {2001, 2010},
    {2001, 2013},
    {1994, 2010},
    {1990, 2000},
    {1989, 2009},
    {1971, 1996},
    {1988, 2004},
    {1987, 2007},
    {1980, 1997}
  };

  std::vector<std::vector<uint64_t>> uint64_props_iu_2 = {
    {962072874067, 933},
    {1374389534791, 4398046516514},
    {2061584699910, 30786325582603},
    {1511828886129, 24189255818939},
    {687195169734, 30786325585585},
    {1924145751643, 8735},
    {824634129681, 24189255820909},
    {412317270574, 32985348834100},
    {412317270523, 28587302324474},
    {274878318222, 30786325579940},
    {274878318393, 32985348839609},
    {1786706808159, 933},
    {549756226945, 4398046521458},
    {274878320024, 6597069774386},
    {1236950997907, 28587302326516},
    {549756231266, 4398046519071},
    {687195185090, 1050},
    {549756233724, 4008},
    {1374389955160, 2199023260990},
    {1236951003301, 4398046521879}
  };

  std::vector<std::vector<std::string>> str_props_iu_2 = {
    {"2012-04-14T01:51:21.746+0000"}, {"2014-02-14T01:51:21.746+0000"}, {"2000-02-14T01:51:21.746+0000"},
    {"2012-02-14T01:51:21.746+0000"}, {"2016-02-14T01:51:21.746+0000"}, {"1999-02-14T01:51:21.746+0000"},
    {"1989-02-14T01:51:21.746+0000"}, {"1981-02-14T01:51:21.746+0000"}, {"1983-02-14T01:51:21.746+0000"},
    {"1993-02-14T01:51:21.746+0000"}, {"1995-02-14T01:51:21.746+0000"}, {"1996-02-14T01:51:21.746+0000"},
    {"1999-02-14T01:51:21.746+0000"}, {"2010-02-14T01:51:21.746+0000"}, {"2010-02-14T01:51:21.746+0000"},
    {"2014-02-14T01:51:21.746+0000"}, {"2013-02-14T01:51:21.746+0000"}, {"2011-02-14T01:51:21.746+0000"},
    {"2013-02-14T01:51:21.746+0000"}, {"2000-02-14T01:51:21.746+0000"}
  };

  std::vector<std::vector<int>> int_props_iu_2 = {};

  std::vector<std::vector<uint64_t>> uint64_props_iu_3 = {
    {1236950871584, 8796093026886},
    {2061584593803, 10995116283227},
    {1786706688521, 19791209308983},
    {1649267736072, 13194139540404},
    {1649267736843, 512},
    {2061584599603, 35184372093792},
    {1099511928595, 15393162795008},
    {412317165378, 3325},
    {2061584610800, 933},
    {1649267752192, 19791209310439},
    {1649267753602, 30786325584319},
    {824634033764, 6597069777216},
    {962072988978, 32985348841418},
    {824634036612, 21990232564124},
    {1374389852068, 4398046514155},
    {1786706713284, 13194139535993},
    {1236950900888, 28587302323441},
    {1511828809498, 8796093030808},
    {1924145672762, 32985348843050},
    {1374389860075, 9616}
  };

  std::vector<std::vector<std::string>> str_props_iu_3 = {
    {"2012-04-14T01:51:21.746+0000"}, {"2012-04-14T01:51:21.746+0000"}, {"2012-04-14T01:51:21.746+0000"},
    {"2012-04-14T01:51:21.746+0000"}, {"2012-04-14T01:51:21.746+0000"}, {"2012-04-14T01:51:21.746+0000"},
    {"2012-04-14T01:51:21.746+0000"}, {"2012-04-14T01:51:21.746+0000"}, {"2012-04-14T01:51:21.746+0000"},
    {"2012-04-14T01:51:21.746+0000"}, {"2012-04-14T01:51:21.746+0000"}, {"2012-04-14T01:51:21.746+0000"},
    {"2012-04-14T01:51:21.746+0000"}, {"2012-04-14T01:51:21.746+0000"}, {"2012-04-14T01:51:21.746+0000"},
    {"2012-04-14T01:51:21.746+0000"}, {"2012-04-14T01:51:21.746+0000"}, {"2012-04-14T01:51:21.746+0000"},
    {"2012-04-14T01:51:21.746+0000"}, {"2012-04-14T01:51:21.746+0000"}
  };

  std::vector<std::vector<int>> int_props_iu_3 = {};

  std::vector<std::vector<uint64_t>> uint64_props_iu_4 = {
    {8796093026886, 7164, 9999999999},
    {4001, 6, 9998888999},
    {24189255816341, 12992, 9990009999},
    {32985348837988, 15672, 9993339999},
    {4398046512636, 1895, 99334569999},
    {8796093032603, 9769, 9677799999},
    {8796093029724, 12700, 87654567999},
    {10995116287090, 16079, 99967654999},
    {2199023256684, 0, 1111111111},
    {32985348840140, 14348, 2345676543},
    {28587302330205, 16028, 8746364784},
    {32985348844015, 6797, 5674337488},
    {19791209300787, 14624, 7583536876},
    {19791209309916, 8615, 57447677474},
    {19791209306932, 4500, 637475895858},
    {35184372093792, 2964, 748473647636},
    {1129, 1014, 84736345757},
    {24189255818615, 12182, 73564747477},
    {10995116282209, 14693, 477574364574},
    {2199023265882, 15987, 9991100009}
  };

  std::vector<std::vector<std::string>> str_props_iu_4 = {
    {"Album 112 of Peter Jones", "2015-12-21T12:50:35.556+0100"}, {"Album 2222 of abcde", "2015-10-29T12:50:35.556+0660"},
    {"Album 22 of fefdsfasdas", "2015-011-15T12:50:35.556+0160"}, {"Album 66 of john", "2015-01-24T12:50:66.556+0100"},
    {"Album 55 of willaims", "2015-06-124T12:50:66.556+0100"}, {"Album 75 of Elizibeth", "2015-06-24T12:50:88.556+0100"},
    {"Album 75 of Samuel", "2015-06-24T12:45:35.556+0100"}, {"Album 89 of Micheal", "2015-05-24T12:50:35.556+0100"},
    {"Album 109 of Mary", "2007-06-22T12:10:35.556+0100"}, {"Album 99 of Silva", "2017-12-24T12:50:35.666+0100"},
    {"Album 12 of Paul john adams", "2001-11-17T12:50:35.576+0100"}, {"Album 33 of Paul", "2016-10-24T12:50:35.556+0600"},
    {"Album 1 of Albert", "1998-06-20T12:50:35.56+0100"}, {"Album 33 of samuel", "1999-07-19T12:60:65.556+0100"},
    {"Album 12 of Alejandro Araya", "2000-05-24T12:50:35.566+0100"}, {"Album 78371 of xyz", "2001-06-24T12:50:65.556+0160"},
    {"Album 1 of Peter Jones", "2011-06-24T12:50:35.556+0100"}, {"Album 56 of Peter Jones", "2012-05-22T12:50:35.556+0100"},
    {"Album 45 of Peter Jones", "2013-03-21T12:50:65.556+0100"}, {"Album 111 of Peter Jones", "2013-02-20T12:50:35.666+0100"}
  };

  std::vector<std::vector<int>> int_props_iu_4 = {};

  std::vector<std::vector<uint64_t>> uint64_props_iu_5 = {
    {4194, 1786706395137},
    {6597069777454, 1099511698394},
    {28587302324814, 1236950666805},
    {28587302326940, 1236950681265},
    {10995116283196, 1786706430791},
    {2199023258871, 1649267452668},
    {6597069766938, 962072719206},
    {6597069773271, 687194836136},
    {32985348838262, 1924145436065},
    {13194139543446, 1236951634156},
    {32985348843050, 1374389560523},
    {30786325588321, 1924145358674},
    {24189255812103, 2061584348659},
    {28587302323229, 824633807283},
    {2199023255685, 1099511714359},
    {8824, 962072697006},
    {7763, 1786706442735},
    {8796093024822, 1374389623873},
    {10995116285557, 1511829542447},
    {933, 2061585359993}
  };

  std::vector<std::vector<std::string>> str_props_iu_5 = {
    {"2011-01-02T06:43:51.955+0000"}, {"2012-03-24T16:37:35.817+0000"}, {"2012-01-17T20:19:22.276+0000"},
    {"2012-01-14T11:14:07.725+0000"}, {"2011-01-02T06:43:51.955+0000"}, {"2011-10-24T06:18:38.139+0000"},
    {"2012-04-09T16:44:42.685+0000"}, {"2011-01-02T06:43:51.955+0000"}, {"2011-01-02T06:43:51.955+0000"},
    {"2012-06-13T03:32:42.855+0000"}, {"2011-01-02T06:43:51.955+0000"}, {"2011-01-02T06:43:51.955+0000"},
    {"2011-01-02T06:43:51.955+0000"}, {"2011-01-02T06:43:51.955+0000"}, {"2011-01-02T06:43:51.955+0000"},
    {"2011-01-02T06:43:51.955+0000"}, {"2011-01-02T06:43:51.955+0000"}, {"2012-06-13T03:32:42.855+0000"},
    {"2011-01-02T06:43:51.955+0000"}, {"2011-07-16T18:37:42.578+0000"}
  };

  std::vector<std::vector<int>> int_props_iu_5 = {};

  std::vector<std::vector<uint64_t>> uint64_props_iu_6 = {
    {10027, 2199024313492, 50, 1679, 999634181970},
    {30786325585301, 2061584333411, 1434, 7513, 9996543581970},
    {10995116279657, 1511828582893, 220, 11072, 999657774570},
    {17592186052886, 412316920493, 1015, 14805, 999664566470},
    {4398046515029, 824633820143, 1315, 6329, 9996561970},
    {32985348834655, 962072743523, 273, 0, 9966664181970},
    {2199023256530, 1099512681152, 991, 16079, 994396660},
    {4398046515173, 412316908754, 1267, 6560, 9994578970},
    {24189255817322, 274877930860, 585, 1100, 999637777970},
    {30786325587134, 2061584395107, 1122, 13489, 999633666970},
    {32985348842038, 47780, 1459, 2214, 999634181970},
    {26388279076840, 1099511715698, 0, 5836, 999466181970},
    {8796093024736, 1786706434469, 919, 15329, 976584181970},
    {21990232556886, 2061584390250, 180, 5081, 999687881970},
    {26388279068827, 962072688578, 1192, 971, 999634181970},
    {6597069774626, 16, 232, 10, 9996341888870},
    {15393162793237, 1099511682067, 767, 7959, 999675474770},
    {6597069777240, 1786706434469, 1216, 13491, 9123456970},
    {30786325580042, 549756869111, 1417, 6361, 9996341234986},
    {13194139535252, 0, 634, 14277, 6576575745}
  };

  std::vector<std::vector<std::string>> str_props_iu_6 = {
    {"", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "english", "About Alexander I of Russia"},
    
    {"", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "Spanish", "About Adolf Hitler, tler's "
    "political views. His writings and meAbout John Milton, eved international renown within his lifetiAbout "
    "Robert Fripp, , a technique often associated with the banAbout Marilyn Monroe"},
    
    {"", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "german", "bout Aurangzeb, ries in the south "
    "expanded the MAbout Peter Hain, ed to his resignation in 2008. HAbout Clement Attlee, came Prime Minister "
    "in 1979. HisAbout Francis Ford Coppola, sese, Terrence Malick, Robert AlAbout Fai|"},
    
    {"", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "Urdu", "About Jim Carrey, ic, slapstick "
    "performaAbout Ludacris,  (2004), Release TheraAbout "},
    
    {"", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "tamil", "About Evo Morales, ma "
    "(born October 26, 1959), popularly known as Evo, is a Bolivian politician and activist, serving as "
    "the 80th President of Bolivia, a position that he has held since 2006. He is also the "},
    
    {"", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "hindi", "About abc def ghi jkl mno pqrs "
    "tuv wxyz ABC DEF GHI JKL MNO PQRS TUV WXYZ !About abc def ghi jkl mno pqrs tuv wxyz ABC DEF GHI JKL MNO "
    "PQRS TUV WXYZ ,About abc def ghi jkl mno pqrs tuv wxyz ABC DEF GHI JKL MNO PQRS TUV WXYZ About abc def "
    "ghi jkl mno pqrs tuv wxyz ABC DEF GHI JKL MNO PQRS TUV WXYZ About abc def ghi jkl mno pqrs tuv wxyz ABC "
    "DEF GHI JKL MNO PQRS TUV WXYZ About abc def ghi jkl mno pqrs tuv wxyz ABC DEF GHI JKL MNO PQRS TUV WXYZ "
    "About abc def ghi jkl mno pqrs tuv wxyz ABC DEF GHI JKL MNO PQRS TUV WXYZ  "},
    
    {"", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "korean", "About Evo Morales, on measures. "
    "Born into a working class Aymara family in Isallawi, Orinoca Canton, Evo grew up aiding hi"},
    
    {"", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "german", "About Lachlan Macquarie, s, "
    "Australia from 1810 to 1821 and had a leading role in the social, economic and architectural "
    "development of the colony. He is c"},
    
    {"", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "english", "About Karl Marx,  where he "
    "became interested in the philosophical ideas of the Young Hegelians. In 1836, he became engaged to "
    "Jenny von Westphalen, marrying her in 1843. After his studies, he wrote for a radical newspaper in "
    "Cologne, and began to w"},
    
    {"", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "arabic", "About Karl Marx, for such goods. "
    "Heavily critical of the current socio-economic form of society, capitalism, he called it the dictatorship "
    "of the bourgeoisie, believing it t"},
    
    {"", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "arabic", "About Brian Mulroney, es Tax, and "
    "the rejection of constitutional reforms such as the ids tht is the again"},
    
    {"", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "english", "About Ivan Ljubičić, s. Ljubičić "
    "used the Head Youtek Extreme Pro Racquet, after using the Babolat Pure DrAbout Dangerous and Moving, "
    "released on October 5, 2005 in Japan"},
    
    {"", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "german", "red Croatian tennis player. "
    "His career-high ATP ranking was no. 3. Tall and powerfully built, he was notedAbout The Rubberband Man,  "
    "100, and topped the U.S. R&B chart at the end of 1976. It"},
    
    {"", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "french", "About Arnold Schwarzenegger, "
    "Governor and Terminator). As a Republican, he was first elected on October 7, 2003, in a special recall "
    "election to replace then-Governor Gray Davis. Schwarzenegger was sworn in"},
    
    {"", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "german", "About Sammy Sosa, Sosa has long been "
    "thAbout William Penn, f William Penn, foundAbout Magical Mystery Tour,  of the record"},
    
    {"", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "malayalam", "About Alexander Downer, s also the "
    "Leader of the Opposition for eight months frAbout Bourbon Restoration, eon (1804–1814/1815) – when a "
    "coalition of European powAbo"},
    
    {"", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "kannada", "About Napoleon, continuation of "
    "the wars sparked by the French Revolution of 1789, they revolAbout Jamie Foxx, n December 13, 1967), "
    "professionally know"},
    
    {"", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "french", "About Augustine of Hippo,  "
    "theology, accommodating a variety of methods and different pers"},
    
    {"", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "telugu", "Francis of Assisi, went to "
    "Egypt in anAbout John Stuart Mill, onception of libertAbout Robert Fripp, p"},
    
    {"", "2011-04-07T14:52:27.809+0000", "46.19.159.176", "Firefox", "japanese", "About Camille Saint-Saëns, "
    "16 December 1921) was About David Hume, lly on utilitariani"},
  };

  std::vector<std::vector<int>> int_props_iu_6 = {
    {123},
    {424},
    {443},
    {434},
    {98},
    {777},
    {234},
    {789},
    {907},
    {445},
    {23},
    {434},
    {244},
    {45},
    {55},
    {987},
    {12},
    {24},
    {443},
    {989}
  };

  std::vector<std::vector<uint64_t>> uint64_props_iu_7 = {
    {19791209305763, 1786707069811, 1459, 1426, 10997654432472},
    {24189255819940, 412316984810, 805, 9, 12343452472},
    {24189255818806, 1649267467066, 1235, 8808, 765456776543},
    {15393162791859, 1374390139472, 20, 7237, 2893289859202},
    {19791209302403, 412319660593, 796, 13525, 95572870933},
    {13194139538552, 1236950581248, 315, 7380, 95957502},
    {8796093026304, 1786708729389, 53, 7534, 95959342},
    {26388279077346, 962073135697, 959, 2493, 6602232},
    {4398046517791, 2176481, 1309, 13150, 8765432456},
    {6597069773790, 2061586938095, 1434, 6452, 4009491401},
    {345, 2199024637094, 585, 1808, 929848909740},
    {8796093029498, 3, 180, 10432, 10432},
    {28587302329250, 412323272449, 1241, 14790, 75891334},
    {8796093028204, 549758755972, 1111, 4939, 589254324982},
    {26388279072962, 1511828865755, 605, 1236, 9570000042325},
    {94, 1924145860444, 30, 8341, 8505838595953},
    {19791209303234, 1374390532480, 215, 12325, 574394839},
    {8796093025123, 824635868714, 1130, 7066, 987503453},
    {6597069773503, 1924149676560, 449, 11251, 86099534},
    {21990232559210, 1786712995900, 50, 0, 100918497450}
  };

  std::vector<std::vector<std::string>> str_props_iu_7 = {
    {"2012-01-09T11:49:15.991+0000", "91.145.169.27", "safari", "About Chen Shui-bian, e Legislative "
    "YuanAbout Éamon de Valera, Pat Coogan sees hiAbout"},
    
    {"2012-07-20T04:51:23.927+0000", "204.79.193.83", "explorer", "About Love to Love You Baby, irst "
    "to be released internationally and in the United Sta"},
    
    {"2011-10-07T04:46:03.896+0000", "91.14.169.237", "safari", "About Ashoka, oka, his legend is "
    "relateAbout Theodore Roosevelt, he Russo-Japanese War, foAbout Ma"},
    
    {"2012-01-09T11:49:15.991+0000", "91.159.169.22", "chrome", "About Edgar Allan Poe, is father "
    "abandoned the family. Poe was tAbout Khanate of"},
    
    {"2010-11-02T01:06:54.399+0000", "91.1467.169.207", "safari", "About Elena Likhovtseva, "
    "rter-finals at the Sony Ericsson Open in MiaAbout Please Come|"},
    
    {"2012-01-09T11:49:15.991+0000", "91.33.169.255", "firefox", "About Charles V, Holy "
    "Roman Emperor, the New WorlAbout Freddie Mercury, ock star. InAbout Al Gore, 0. Victory iAbout"},
    
    {"2012-01-09T11:49:15.991+0000", "91.149.11.27", "safari", "About Imelda Marcos, ected as "
    "membAbout Dead Leaves and the Dirty Ground, e UK Albums"},
    
    {"2012-01-09T11:49:15.991+0000", "91.149.169.2237", "safari", "About John Howard, died in "
    "combaAbout Isabella I of Castile, r right to thAbo"},
    
    {"2012-01-09T11:49:15.991+0000", "91.149.169.27", "netscape", "About Imelda Marcos, tatives "
    "to represent Ilocos Norte's seAbout Sultanate"},
    
    {"2012-01-09T11:49:15.991+0000", "204.79.194.82", "safari", "About Germany, hird largest "
    "importer of goods. The country has developedAbout Kingdom of Kandy, kept European col"},
    
    {"2012-01-09T11:49:15.991+0000", "202.719.194.222", "firefox", "Internet Explorer|About "
    "Jorge Luis Borges, by the LatinAbout Arnold Schoenberg, uermann, and About"},
    
    {"2012-01-09T11:49:15.991+0000", "41.149.119.247", "safari", "About Georg Wilhelm "
    "Friedrich Hegel, that mind or spirit manifested itself in Ab"},
    
    {"2012-01-09T11:49:15.991+0000", "41.149.169.27", "safari", "About Hey Sexy Lady, "
    "the title of a song recorded by Jamacian-American reggae artist ShaggAbout Brown "
    "Album, nder; as such, this is their"},
    
    {"2012-01-09T11:49:15.991+0000", "91.149.169.27", "mozilla", "About George Frideric "
    "Handel, s oratorios is an ethical one. They aAbout Jay-Z, ey Carter (born December "
    "4, 1969), beAbout Garth Brooks, ted int"},
    
    {"2012-01-09T11:49:15.991+0000", "88.19.169.217", "safari", "About Bruce Lee,  "
    "Hong Kong martial arts filmAbout Greece, irthplace of democracy"},
    
    {"2012-01-09T11:49:15.991+0000", "294.34.294.10", "safari", "About Nero, t. "
    "The study of NeroAbout Eleanor Rigby, ing quartet arrangemAbout New Zealand,  the New Zealand ArmAbou"},
    
    {"2011-10-08T14:00:21.609+0000", "91.149.129.237", "safari", "bout Amy Winehouse,  Love Is a Losing "
    "Game. Winehouse died of alcAbout Where Is the Love"},
    
    {"2012-05-13T13:41:59.023+0000", "234.22.194.42", "chrome", "About William IV of the United "
    "Kingdom, ribbean, but saw little actual fighting. SA"},
    
    {"2011-10-18T10:27:27.756+0000", "91.149.169.27", "safari", "About Carl Jung, while still "
    "mAbout Katy Perry, five number oAbout Brian W"},
    
    {"2011-06-29T00:05:26.944+0000", "266.79.14.02", "Opera", "|About Marcelo Melo, he "
    "seventh BrazilianAbout Pope Pius XI, e Mystici Corpori"}
  };

  std::vector<std::vector<int>> int_props_iu_7 = {
    {54}, {78},
    {75}, {543},
    {343}, {99},
    {31}, {89},
    {90}, {353},
    {233}, {89},
    {67}, {67},
    {33}, {123},
    {689}, {77},
    {12}, {79}
  };

  std::vector<std::vector<uint64_t>> uint64_props_iu_8 = {
    {24189255811086, 555},
    {24189255814147, 15393162793955},
    {21990232556891, 4398046522002},
    {933, 35184372093792},
    {8796093030323, 9005},
    {8649, 4398046521818},
    {15393162796413, 15393162794170},
    {28587302324497, 15393162797671},
    {8796093030039, 24189255819297},
    {30786325581382, 28587302328644},
    {30786325578904, 30786325588071},
    {35184372097876, 10027},
    {13194139535717, 17592186044551},
    {2199023261114, 933},
    {2199023266429, 32985348835435},
    {24189255819114, 32985348838898},
    {6478, 28587302322743},
    {30786325580396, 2862},
    {15393162799139, 17592186051428},
    {4398046514193, 32985348838643},
  };

  std::vector<std::vector<std::string>> str_props_iu_8 = {
    {"2011-11-21T02:27:05.636+0000"}, {"2010-03-12T17:28:43.563+0000"}, {"2011-12-10T17:41:10.929+0000"},
    {"1999-02-26T21:37:35.528+0000"}, {"2012-07-20T16:42:08.327+0000"}, {"2010-12-09T18:13:19.867+000"},
    {"2011-05-03T18:56:54.450+0000"}, {"2011-03-19T05:23:21.691+0000"}, {"2011-01-30T00:44:45.595+0000"},
    {"2010-12-15T23:27:00.248+0000"}, {"2010-04-13T16:23:31.692+0000"}, {"2010-07-15T04:56:46.299+0000"},
    {"2012-07-11T03:57:44.959+0000"}, {"2012-02-01T03:30:59.228+0001"}, {"2010-07-16T03:52:16.375+0000"},
    {"2010-06-08T07:34:47.276+0000"}, {"2002-03-03T19:22:42.796+0000"}, {"2011-12-05T07:04:55.336+0000"},
    {"2010-07-08T13:44:13.873+0000"}, {"2019-06-03T06:17:49.321+0000"}
  };

  std::vector<std::vector<int>> int_props_iu_8 = {};

  load_snb_data(graph, node_files, rship_files);
  std::cout << "\n\n";

  auto i = 0;
  for (auto ids : uint64_props_iu_1){
    auto start_qp = std::chrono::steady_clock::now();
    //ldbc_is_query_1(graph, rs_is_1[i++], id);
    ldbc_iu_query_1(graph, ids, str_props_iu_1[i], int_props_iu_1[i]);
    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "ldbc_iu_query_1 executed in "
                << std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp).count()
                << " μs" << std::endl;
    i++;
  }
  std::cout << "\n\n";

  i = 0;
  for (auto ids : uint64_props_iu_2){
    auto start_qp = std::chrono::steady_clock::now();
    //ldbc_is_query_1(graph, rs_is_1[i++], id);
    ldbc_iu_query_2(graph, ids, str_props_iu_2[i], int_props_iu_2[i]);
    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "ldbc_iu_query_2 executed in "
                << std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp).count()
                << " μs" << std::endl;
    i++;
  }
  std::cout << "\n\n";

  i = 0;
  for (auto ids : uint64_props_iu_3){
    auto start_qp = std::chrono::steady_clock::now();
    //ldbc_is_query_1(graph, rs_is_1[i++], id);
    ldbc_iu_query_3(graph, ids, str_props_iu_3[i], int_props_iu_3[i]);
    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "ldbc_iu_query_3 executed in "
                << std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp).count()
                << " μs" << std::endl;
    i++;
  }
  std::cout << "\n\n";

  i = 0;
  for (auto ids : uint64_props_iu_4){
    auto start_qp = std::chrono::steady_clock::now();
    //ldbc_is_query_1(graph, rs_is_1[i++], id);
    ldbc_iu_query_4(graph, ids, str_props_iu_4[i], int_props_iu_4[i]);
    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "ldbc_iu_query_4 executed in "
                << std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp).count()
                << " μs" << std::endl;
    i++;
  }
  std::cout << "\n\n";

  i = 0;
  for (auto ids : uint64_props_iu_5){
    auto start_qp = std::chrono::steady_clock::now();
    //ldbc_is_query_1(graph, rs_is_1[i++], id);
    ldbc_iu_query_5(graph, ids, str_props_iu_5[i], int_props_iu_5[i]);
    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "ldbc_iu_query_5 executed in "
                << std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp).count()
                << " μs" << std::endl;
    i++;
  }
  std::cout << "\n\n";

  i = 0;
  for (auto ids : uint64_props_iu_6){
    auto start_qp = std::chrono::steady_clock::now();
    //ldbc_is_query_1(graph, rs_is_1[i++], id);
    ldbc_iu_query_6(graph, ids, str_props_iu_6[i], int_props_iu_6[i]);
    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "ldbc_iu_query_6 executed in "
                << std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp).count()
                << " μs" << std::endl;
    i++;
  }
  std::cout << "\n\n";

  i = 0;
  for (auto ids : uint64_props_iu_7){
    auto start_qp = std::chrono::steady_clock::now();
    //ldbc_is_query_1(graph, rs_is_1[i++], id);
    ldbc_iu_query_7(graph, ids, str_props_iu_7[i], int_props_iu_7[i]);
    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "ldbc_iu_query_7 executed in "
                << std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp).count()
                << " μs" << std::endl;
    i++;
  }
  std::cout << "\n\n";

  i = 0;
  for (auto ids : uint64_props_iu_8){
    auto start_qp = std::chrono::steady_clock::now();
    //ldbc_is_query_1(graph, rs_is_1[i++], id);
    ldbc_iu_query_8(graph, ids, str_props_iu_8[i], int_props_iu_8[i]);
    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "ldbc_iu_query_8 executed in "
                << std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp).count()
                << " μs" << std::endl;
    i++;
  }
  std::cout << "\n\n";

#ifdef USE_TX
  graph->commit_transaction();
#endif

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}

/*TEST_CASE("ldbc_is_query_1", "[ldbc]") {
#ifdef USE_PMDK
  if (access(test_path.c_str(), F_OK) == 0)
    remove(test_path.c_str());
  auto pop = prepare_pool();
  auto graph = create_graph(pop);
#else
  auto graph = create_graph();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                          snb_sta + "place_0_0.csv", snb_sta + "place_1_0.csv",
                                          snb_sta + "place_2_0.csv", snb_sta + "place_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "person_isLocatedIn_place_0_0.csv",
                                          snb_dyn + "person_isLocatedIn_place_1_0.csv",
                                          snb_dyn + "person_isLocatedIn_place_2_0.csv",
                                          snb_dyn + "person_isLocatedIn_place_3_0.csv"};

  std::vector<uint64_t> personIds = {933, 24189255812290, 6597069773744, 2199023266220, 13194139544176,
                                      17592186050570, 24189255815734, 28587302330379, 32985348842922, 3601,
                                      4398046511870, 32985348834284, 17592186045096, 17592186053245, 4398046520495,
                                      4233, 344, 10995116286457, 10976, 24189255813927};
  
  load_snb_data(graph, node_files, rship_files);
  std::cout << "\n\n";
  auto i = 0;
  result_set rs[20];
  
  for (auto id : personIds){
    auto start_qp = std::chrono::steady_clock::now();
    ldbc_is_query_1(graph, rs[i++], id);
    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "ldbc_is_query_1 with PersonId " << id << " executed in "
                << std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp).count()
                << " μs" << std::endl;
  }
  std::cout << "\n\n";
  for (auto r : rs)
    REQUIRE(!r.data.empty()); //std::cout << r << "\n";

#ifdef USE_TX
  graph->commit_transaction();
#endif

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}

TEST_CASE("ldbc_is_query_2", "[ldbc]") {
#ifdef USE_PMDK
  if (access(test_path.c_str(), F_OK) == 0)
    remove(test_path.c_str());
  auto pop = prepare_pool();
  auto graph = create_graph(pop);
#else
  auto graph = create_graph();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                          snb_dyn + "post_0_0.csv", snb_dyn + "post_1_0.csv",
                                          snb_dyn + "post_2_0.csv", snb_dyn + "post_3_0.csv",
                                          snb_dyn + "comment_0_0.csv", snb_dyn + "comment_1_0.csv",
                                          snb_dyn + "comment_2_0.csv", snb_dyn + "comment_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "post_hasCreator_person_0_0.csv",
                                          snb_dyn + "post_hasCreator_person_1_0.csv",
                                          snb_dyn + "post_hasCreator_person_2_0.csv",
                                          snb_dyn + "post_hasCreator_person_3_0.csv",
                                          snb_dyn + "comment_hasCreator_person_0_0.csv",
                                          snb_dyn + "comment_hasCreator_person_1_0.csv",
                                          snb_dyn + "comment_hasCreator_person_2_0.csv",
                                          snb_dyn + "comment_hasCreator_person_3_0.csv",
                                          snb_dyn + "comment_replyOf_post_0_0.csv",
                                          snb_dyn + "comment_replyOf_post_1_0.csv",
                                          snb_dyn + "comment_replyOf_post_2_0.csv",
                                          snb_dyn + "comment_replyOf_post_3_0.csv",
                                          snb_dyn + "comment_replyOf_comment_0_0.csv",
                                          snb_dyn + "comment_replyOf_comment_1_0.csv",
                                          snb_dyn + "comment_replyOf_comment_2_0.csv",
                                          snb_dyn + "comment_replyOf_comment_3_0.csv"};

  std::vector<uint64_t> personIds = {65, 28587302330379, 3601, 24189255817217, 4398046511870,
                                      8698, 6597069773744, 13194139544176, 17592186050570, 6597069766993,
                                      24189255815734, 26388279077330, 15393162799262, 32985348843825, 32985348843760,
                                      32985348842653, 13194139540894, 13194139540856, 8796093028361, 6597069766998};
  
  load_snb_data(graph, node_files, rship_files);
  std::cout << "\n\n";
  auto i = 0;
  result_set rs[20];
  
  for (auto id : personIds){
    auto start_qp = std::chrono::steady_clock::now();
    ldbc_is_query_2(graph, rs[i++], id);
    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "ldbc_is_query_2 with PersonId " << id << " executed in "
                << std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp).count()
                << " μs" << std::endl;
  }
  std::cout << "\n\n";
  for (auto r : rs)
    REQUIRE(!r.data.empty()); //std::cout << r << "\n";

#ifdef USE_TX
  graph->commit_transaction();
#endif

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}

TEST_CASE("ldbc_is_query_3", "[ldbc]") {
#ifdef USE_PMDK
  if (access(test_path.c_str(), F_OK) == 0)
    remove(test_path.c_str());
  auto pop = prepare_pool();
  auto graph = create_graph(pop);
#else
  auto graph = create_graph();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "person_knows_person_0_0.csv",
                                          snb_dyn + "person_knows_person_1_0.csv",
                                          snb_dyn + "person_knows_person_2_0.csv",
                                          snb_dyn + "person_knows_person_3_0.csv"};

  std::vector<uint64_t> personIds = {19791209304051, 28587302326940, 2199023262021, 8796093027111, 2199023262994,
                                      6597069773744, 13194139544176, 17592186050570, 30786325588658, 24189255815734,
                                      6597069774931, 13194139544258, 15393162791382, 21990232558836, 28587302322686,
                                      24189255820923, 32985348833548, 30786325581208, 26388279074032, 32985348834375};
  
  load_snb_data(graph, node_files, rship_files);
  std::cout << "\n\n";
  auto i = 0;
  result_set rs[20];
  
  for (auto id : personIds){
    auto start_qp = std::chrono::steady_clock::now();
    ldbc_is_query_3(graph, rs[i++], id);
    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "ldbc_is_query_3 with PersonId " << id << " executed in "
                << std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp).count()
                << " μs" << std::endl;
  }
  std::cout << "\n\n";
  for (auto r : rs)
    REQUIRE(!r.data.empty()); //std::cout << r << "\n";

#ifdef USE_TX
  graph->commit_transaction();
#endif

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}

TEST_CASE("ldbc_is_query_4", "[ldbc]") {
#ifdef USE_PMDK
  if (access(test_path.c_str(), F_OK) == 0)
    remove(test_path.c_str());
  auto pop = prepare_pool();
  auto graph = create_graph(pop);
#else
  auto graph = create_graph();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  std::vector<std::string> node_files = {snb_dyn + "post_0_0.csv", snb_dyn + "post_1_0.csv",
                                          snb_dyn + "post_2_0.csv", snb_dyn + "post_3_0.csv",
                                          snb_dyn + "comment_0_0.csv", snb_dyn + "comment_1_0.csv",
                                          snb_dyn + "comment_2_0.csv", snb_dyn + "comment_3_0.csv"};

  std::vector<std::string> rship_files = {};

  std::vector<uint64_t> PostIds = {1374389534801, 687194926510, 1236950581577, 824633724379, 687194903818,
                                      549755930326, 1649267546616, 1649267453265, 1924145376549, 1099511719169};

  std::vector<uint64_t> CommentIds = {1236950581249, 1374389535139, 687194767797, 962072674365, 274877974096,
                                      1374389620660, 1374389535186, 2061584302604, 1099511678319, 1099511755889};
  
  load_snb_data(graph, node_files, rship_files);
  std::cout << "\n\n";
  auto i = 0;
  result_set rs[10];
  
  for (auto id : CommentIds){
    auto start_qp = std::chrono::steady_clock::now();
    ldbc_is_query_4(graph, rs[i++], id);
    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "ldbc_is_query_4 with CommentId " << id << " executed in "
                << std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp).count()
                << " μs" << std::endl;
  }
  std::cout << "\n\n";
  for (auto r : rs)
    REQUIRE(!r.data.empty()); //std::cout << r << "\n";

#ifdef USE_TX
  graph->commit_transaction();
#endif

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}

TEST_CASE("ldbc_is_query_5", "[ldbc]") {
#ifdef USE_PMDK
  if (access(test_path.c_str(), F_OK) == 0)
    remove(test_path.c_str());
  auto pop = prepare_pool();
  auto graph = create_graph(pop);
#else
  auto graph = create_graph();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                          snb_dyn + "post_0_0.csv", snb_dyn + "post_1_0.csv",
                                          snb_dyn + "post_2_0.csv", snb_dyn + "post_3_0.csv",
                                          snb_dyn + "comment_0_0.csv", snb_dyn + "comment_1_0.csv",
                                          snb_dyn + "comment_2_0.csv", snb_dyn + "comment_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "post_hasCreator_person_0_0.csv",
                                          snb_dyn + "post_hasCreator_person_1_0.csv",
                                          snb_dyn + "post_hasCreator_person_2_0.csv",
                                          snb_dyn + "post_hasCreator_person_3_0.csv",
                                          snb_dyn + "comment_hasCreator_person_0_0.csv",
                                          snb_dyn + "comment_hasCreator_person_1_0.csv",
                                          snb_dyn + "comment_hasCreator_person_2_0.csv",
                                          snb_dyn + "comment_hasCreator_person_3_0.csv"};

  std::vector<uint64_t> PostIds = {1649267611029, 1649267641500, 1649267717129, 549756117312, 962073027971,
                                      1924145709571, 1786706759766, 137439322338, 962073047211, 1786706792809};

  std::vector<uint64_t> commentIds = {2061584429975, 1099511764068, 1511828638961, 1099511794459, 1924145529653,
                                      137439153914, 1374389758562, 687194998602, 1099511869402, 1649267722310};
  
  load_snb_data(graph, node_files, rship_files);
  std::cout << "\n\n";
  auto i = 0;
  result_set rs[10];
  
  for (auto id : commentIds){
    auto start_qp = std::chrono::steady_clock::now();
    ldbc_is_query_5(graph, rs[i++], id);
    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "ldbc_is_query_5 with commentId " << id << " executed in "
                << std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp).count()
                << " μs" << std::endl;
  }
  std::cout << "\n\n";
  for (auto r : rs)
    REQUIRE(!r.data.empty()); //std::cout << r << "\n";

#ifdef USE_TX
  graph->commit_transaction();
#endif

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}

TEST_CASE("ldbc_is_query_6", "[ldbc]") {
#ifdef USE_PMDK
  if (access(test_path.c_str(), F_OK) == 0)
    remove(test_path.c_str());
  auto pop = prepare_pool();
  auto graph = create_graph(pop);
#else
  auto graph = create_graph();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                          snb_dyn + "post_0_0.csv", snb_dyn + "post_1_0.csv",
                                          snb_dyn + "post_2_0.csv", snb_dyn + "post_3_0.csv",
                                          snb_dyn + "comment_0_0.csv", snb_dyn + "comment_1_0.csv",
                                          snb_dyn + "comment_2_0.csv", snb_dyn + "comment_3_0.csv",
                                          snb_dyn + "forum_0_0.csv", snb_dyn + "forum_1_0.csv",
                                          snb_dyn + "forum_2_0.csv", snb_dyn + "forum_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "comment_replyOf_post_0_0.csv",
                                          snb_dyn + "comment_replyOf_post_1_0.csv",
                                          snb_dyn + "comment_replyOf_post_2_0.csv",
                                          snb_dyn + "comment_replyOf_post_3_0.csv",
                                          snb_dyn + "comment_replyOf_comment_0_0.csv",
                                          snb_dyn + "comment_replyOf_comment_1_0.csv",
                                          snb_dyn + "comment_replyOf_comment_2_0.csv",
                                          snb_dyn + "comment_replyOf_comment_3_0.csv",
                                          snb_dyn + "forum_containerOf_post_0_0.csv",
                                          snb_dyn + "forum_containerOf_post_1_0.csv",
                                          snb_dyn + "forum_containerOf_post_2_0.csv",
                                          snb_dyn + "forum_containerOf_post_3_0.csv",
                                          snb_dyn + "forum_hasModerator_person_0_0.csv",
                                          snb_dyn + "forum_hasModerator_person_1_0.csv",
                                          snb_dyn + "forum_hasModerator_person_2_0.csv",
                                          snb_dyn + "forum_hasModerator_person_3_0.csv"};

  std::vector<uint64_t> PostIds = {1374389534795, 3, 246, 1786710746552, 1786710746860,
                                      962077492609, 4818574, 137443772206, 4818783, 1649273779906,
                                      1099512706784, 1924145709571, 274879100510, 2061585683162, 2061585683383,
                                      824638318943, 962073868902, 962076990540, 1236955780271, 1924151699930};
  std::vector<uint64_t> commentIds = {549756150652, 2061587049723, 1786710610862, 1924150141935, 1649271672251,
                                      1099518023455, 1511835112930, 962079298952, 549762439424, 1786707596571,
                                      824635086444, 2199024637100, 549762296256, 412319368884, 1924148311956,
                                      687196868319, 1786710956334, 2882812, 274878321446, 687194840176};
  
  load_snb_data(graph, node_files, rship_files);
  std::cout << "\n\n";
  auto i = 0;
  result_set rs[20];
  
  for (auto id : commentIds){
    auto start_qp = std::chrono::steady_clock::now();
    ldbc_is_query_6(graph, rs[i], id);
    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "ldbc_is_query_6 with commentId " << id << " executed in "
                << std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp).count()
                << " μs" << std::endl;
    i++;
  }
  std::cout << "\n\n";
  for (auto r : rs)
    REQUIRE(!r.data.empty()); //std::cout << r << "\n";

#ifdef USE_TX
  graph->commit_transaction();
#endif

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}

TEST_CASE("ldbc_is_query_7", "[ldbc]") {
#ifdef USE_PMDK
  if (access(test_path.c_str(), F_OK) == 0)
    remove(test_path.c_str());
  auto pop = prepare_pool();
  auto graph = create_graph(pop);
#else
  auto graph = create_graph();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                          snb_dyn + "comment_0_0.csv", snb_dyn + "comment_1_0.csv",
                                          snb_dyn + "comment_2_0.csv", snb_dyn + "comment_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "comment_hasCreator_person_0_0.csv",
                                          snb_dyn + "comment_hasCreator_person_1_0.csv",
                                          snb_dyn + "comment_hasCreator_person_2_0.csv",
                                          snb_dyn + "comment_hasCreator_person_3_0.csv",
                                          snb_dyn + "comment_replyOf_comment_0_0.csv",
                                          snb_dyn + "comment_replyOf_comment_1_0.csv",
                                          snb_dyn + "comment_replyOf_comment_2_0.csv",
                                          snb_dyn + "comment_replyOf_comment_3_0.csv",
                                          snb_dyn + "person_knows_person_0_0.csv",
                                          snb_dyn + "person_knows_person_1_0.csv",
                                          snb_dyn + "person_knows_person_2_0.csv",
                                          snb_dyn + "person_knows_person_3_0.csv"};

  std::vector<uint64_t> commentIds = {549755814584, 962074006383, 1374390866272, 1511833539098, 687196097161,
                                      1786710956803, 1924148155034, 824636527214, 2061587107320, 274880712071,
                                      962075482675, 1786708701848, 2061588922925, 4784850, 4784913,
                                      412321645469, 1374394320184, 1374390902281, 1511834991008, 824634964783};
  
  load_snb_data(graph, node_files, rship_files);
  std::cout << "\n\n";
  auto i = 0;
  result_set rs[20];
  
  for (auto id : commentIds){
    auto start_qp = std::chrono::steady_clock::now();
    ldbc_is_query_7(graph, rs[i], id);
    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "ldbc_is_query_7 with commentId " << id << " executed in "
                << std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp).count()
                << " μs" << std::endl;
    i++;
  }
  std::cout << "\n\n";
  for (auto r : rs)
    std::cout << r << "\n"; //REQUIRE(!r.data.empty()); //std::cout << r << "\n";

#ifdef USE_TX
  graph->commit_transaction();
#endif

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}

TEST_CASE("LDBC Interactive Short Query 1", "[ldbc]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto graph = create_graph(pop);
#else
  auto graph = create_graph();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                          snb_sta + "place_0_0.csv", snb_sta + "place_1_0.csv",
                                          snb_sta + "place_2_0.csv", snb_sta + "place_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "person_isLocatedIn_place_0_0.csv",
                                          snb_dyn + "person_isLocatedIn_place_1_0.csv",
                                          snb_dyn + "person_isLocatedIn_place_2_0.csv",
                                          snb_dyn + "person_isLocatedIn_place_3_0.csv"};

  load_snb_data(graph, node_files, rship_files);

  result_set rs, expected;
  expected.data.push_back(
      {query_result("Mahinda"), query_result("Perera"),
        query_result("1989-12-03"), query_result("119.235.7.103"),
        query_result("Firefox"), query_result("1353"), query_result("male"),
        query_result("2010-02-14T15:32:10.447000")});

  ldbc_is_query_1(graph, rs, 933);

  REQUIRE(rs == expected);

#ifdef USE_TX
  graph->commit_transaction();
#endif

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}

TEST_CASE("LDBC Interactive Short Query 2", "[ldbc]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto graph = create_graph(pop);
#else
  auto graph = create_graph();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                          snb_dyn + "post_0_0.csv", snb_dyn + "post_1_0.csv",
                                          snb_dyn + "post_2_0.csv", snb_dyn + "post_3_0.csv",
                                          snb_dyn + "comment_0_0.csv", snb_dyn + "comment_1_0.csv",
                                          snb_dyn + "comment_2_0.csv", snb_dyn + "comment_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "post_hasCreator_person_0_0.csv",
                                          snb_dyn + "post_hasCreator_person_1_0.csv",
                                          snb_dyn + "post_hasCreator_person_2_0.csv",
                                          snb_dyn + "post_hasCreator_person_3_0.csv",
                                          snb_dyn + "comment_hasCreator_person_0_0.csv",
                                          snb_dyn + "comment_hasCreator_person_1_0.csv",
                                          snb_dyn + "comment_hasCreator_person_2_0.csv",
                                          snb_dyn + "comment_hasCreator_person_3_0.csv",
                                          snb_dyn + "comment_replyOf_post_0_0.csv",
                                          snb_dyn + "comment_replyOf_post_1_0.csv",
                                          snb_dyn + "comment_replyOf_post_2_0.csv",
                                          snb_dyn + "comment_replyOf_post_3_0.csv",
                                          snb_dyn + "comment_replyOf_comment_0_0.csv",
                                          snb_dyn + "comment_replyOf_comment_1_0.csv",
                                          snb_dyn + "comment_replyOf_comment_2_0.csv",
                                          snb_dyn + "comment_replyOf_comment_3_0.csv"};

  load_snb_data(graph, node_files, rship_files);

  result_set rs, expected;
  expected.data.push_back(
      {query_result("2199029789250"), query_result("fine"), query_result("2012-09-13T06:10:24.610000"),
      query_result("2199029789233"), query_result("4398046519022"), 
      query_result("Zheng"), query_result("Xu")});
  expected.data.push_back(
      {query_result("2199023895468"), query_result("About Pope John Paul II, vanni Paolo II,About Alfred, "
        "Lord Tennyson, t / T"), query_result("2012-09-12T10:03:59.850000"),
      query_result("2199023895464"), query_result("21990232556018"), 
      query_result("K."), query_result("Kumar")});
  expected.data.push_back(
      {query_result("2061590835573"), query_result("About Catherine the Great, ge of 67. She was born "
        "in Stettin, PomeraniaAbout Bangladesh"), query_result("2012-08-20T04:48:28.061000"),
      query_result("2061590835571"), query_result("17592186052552"), 
      query_result("A."), query_result("Nair")});
  expected.data.push_back(
      {query_result("2061590836130"), query_result("thanks"), query_result("2012-07-28T00:28:44.339000"),
      query_result("2061590836122"), query_result("15393162794737"), 
      query_result("Wilson"), query_result("Agudelo")});
  expected.data.push_back(
      {query_result("2061590835519"), query_result("LOL"), query_result("2012-07-25T05:05:52.401000"),
      query_result("2061590835503"), query_result("26388279068563"), 
      query_result("Patrick"), query_result("Ambane")});
  expected.data.push_back(
      {query_result("2061590835517"), query_result("yes"), query_result("2012-07-25T02:09:45.010000"),
      query_result("2061590835503"), query_result("26388279068563"), 
      query_result("Patrick"), query_result("Ambane")});
  expected.data.push_back(
      {query_result("962079207711"), query_result("roflol"), query_result("2011-04-12T14:02:40.962000"),
      query_result("962079207708"), query_result("13194139542623"), 
      query_result("Andre"), query_result("Dia")});
  expected.data.push_back(
      {query_result("962079207716"), query_result("yes"), query_result("2011-04-12T11:45:11.947000"),
      query_result("962079207708"), query_result("13194139542623"), 
      query_result("Andre"), query_result("Dia")});
  expected.data.push_back(
      {query_result("962079207714"), query_result("I see"), query_result("2011-04-11T21:22:03.107000"),
      query_result("962079207708"), query_result("13194139542623"), 
      query_result("Andre"), query_result("Dia")});
  expected.data.push_back(
      {query_result("962079207709"), query_result("About Jerry Lewis, r his slapstick humor in film, "
        "television, stage and radio."), query_result("2011-04-11T21:01:46.154000"),
      query_result("962079207708"), query_result("13194139542623"), 
      query_result("Andre"), query_result("Dia")});

  ldbc_is_query_2(graph, rs);

  REQUIRE(rs == expected);

#ifdef USE_TX
  graph->commit_transaction();
#endif

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}

TEST_CASE("LDBC Interactive Short Query 3", "[ldbc]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto graph = create_graph(pop);
#else
  auto graph = create_graph();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "person_knows_person_0_0.csv",
                                          snb_dyn + "person_knows_person_1_0.csv",
                                          snb_dyn + "person_knows_person_2_0.csv",
                                          snb_dyn + "person_knows_person_3_0.csv"};

  load_snb_data(graph, node_files, rship_files);

  result_set rs, expected;
  expected.data.push_back(
      {query_result("32985348833579"), query_result("Otto"),
        query_result("Becker"), query_result("2012-09-07T01:11:30.195000")});
  expected.data.push_back(
      {query_result("32985348838375"), query_result("Otto"),
        query_result("Richter"), query_result("2012-07-17T08:04:49.463000")});
  expected.data.push_back(
      {query_result("10995116284808"), query_result("Andrei"),
        query_result("Condariuc"), query_result("2011-01-02T06:43:41.955000")});
  expected.data.push_back(
      {query_result("6597069777240"), query_result("Fritz"),
        query_result("Muller"), query_result("2010-09-20T09:42:43.187000")});
  expected.data.push_back(
      {query_result("4139"), query_result("Baruch"),
        query_result("Dego"), query_result("2010-03-13T07:37:21.718000")});
  
  ldbc_is_query_3(graph, rs);

  REQUIRE(rs == expected);

#ifdef USE_TX
  graph->commit_transaction();
#endif

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}

TEST_CASE("LDBC Interactive Short Query 4", "[ldbc]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto graph = create_graph(pop);
#else
  auto graph = create_graph();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  std::vector<std::string> node_files = {snb_dyn + "post_0_0.csv", snb_dyn + "post_1_0.csv",
                                          snb_dyn + "post_2_0.csv", snb_dyn + "post_3_0.csv"};

  std::vector<std::string> rship_files = {};

  load_snb_data(graph, node_files, rship_files);

  result_set rs, expected;
  expected.data.push_back(
      {query_result("2011-10-05T14:38:36.019000"),
        query_result("photo1374389534791.jpg")});
  

  ldbc_is_query_4(graph, rs);

  REQUIRE(rs == expected);

#ifdef USE_TX
  graph->commit_transaction();
#endif

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}

TEST_CASE("LDBC Interactive Short Query 5", "[ldbc]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto graph = create_graph(pop);
#else
  auto graph = create_graph();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                          snb_dyn + "comment_0_0.csv", snb_dyn + "comment_1_0.csv",
                                          snb_dyn + "comment_2_0.csv", snb_dyn + "comment_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "comment_hasCreator_person_0_0.csv",
                                          snb_dyn + "comment_hasCreator_person_1_0.csv",
                                          snb_dyn + "comment_hasCreator_person_2_0.csv",
                                          snb_dyn + "comment_hasCreator_person_3_0.csv"};

  load_snb_data(graph, node_files, rship_files);

  result_set rs, expected;
  expected.data.push_back(
      {query_result("10995116284808"),
        query_result("Andrei"),
        query_result("Condariuc")});
  

  ldbc_is_query_5(graph, rs);

  REQUIRE(rs == expected);

#ifdef USE_TX
  graph->commit_transaction();
#endif

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}

TEST_CASE("LDBC Interactive Short Query 6", "[ldbc]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto graph = create_graph(pop);
#else
  auto graph = create_graph();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                          snb_dyn + "post_0_0.csv", snb_dyn + "post_1_0.csv",
                                          snb_dyn + "post_2_0.csv", snb_dyn + "post_3_0.csv",
                                          snb_dyn + "comment_0_0.csv", snb_dyn + "comment_1_0.csv",
                                          snb_dyn + "comment_2_0.csv", snb_dyn + "comment_3_0.csv",
                                          snb_dyn + "forum_0_0.csv", snb_dyn + "forum_1_0.csv",
                                          snb_dyn + "forum_2_0.csv", snb_dyn + "forum_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "comment_replyOf_post_0_0.csv",
                                          snb_dyn + "comment_replyOf_post_1_0.csv",
                                          snb_dyn + "comment_replyOf_post_2_0.csv",
                                          snb_dyn + "comment_replyOf_post_3_0.csv",
                                          snb_dyn + "comment_replyOf_comment_0_0.csv",
                                          snb_dyn + "comment_replyOf_comment_1_0.csv",
                                          snb_dyn + "comment_replyOf_comment_2_0.csv",
                                          snb_dyn + "comment_replyOf_comment_3_0.csv",
                                          snb_dyn + "forum_containerOf_post_0_0.csv",
                                          snb_dyn + "forum_containerOf_post_1_0.csv",
                                          snb_dyn + "forum_containerOf_post_2_0.csv",
                                          snb_dyn + "forum_containerOf_post_3_0.csv",
                                          snb_dyn + "forum_hasModerator_person_0_0.csv",
                                          snb_dyn + "forum_hasModerator_person_1_0.csv",
                                          snb_dyn + "forum_hasModerator_person_2_0.csv",
                                          snb_dyn + "forum_hasModerator_person_3_0.csv"};

  load_snb_data(graph, node_files, rship_files);

  result_set rs, expected;
  expected.data.push_back(
      {query_result("37"),
        query_result("Wall of Hồ Chí Do"),
        query_result("4194"),
        query_result("Hồ Chí"),
        query_result("Do")});
  expected.data.push_back(
      {query_result("37"),
        query_result("Wall of Hồ Chí Do"),
        query_result("4194"),
        query_result("Hồ Chí"),
        query_result("Do")});

  ldbc_is_query_6(graph, rs);
  
  REQUIRE(rs == expected);

#ifdef USE_TX
  graph->commit_transaction();
#endif

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}

TEST_CASE("LDBC Interactive Short Query 7", "[ldbc]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto graph = create_graph(pop);
#else
  auto graph = create_graph();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                          snb_dyn + "comment_0_0.csv", snb_dyn + "comment_1_0.csv",
                                          snb_dyn + "comment_2_0.csv", snb_dyn + "comment_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "comment_hasCreator_person_0_0.csv",
                                          snb_dyn + "comment_hasCreator_person_1_0.csv",
                                          snb_dyn + "comment_hasCreator_person_2_0.csv",
                                          snb_dyn + "comment_hasCreator_person_3_0.csv",
                                          snb_dyn + "comment_replyOf_comment_0_0.csv",
                                          snb_dyn + "comment_replyOf_comment_1_0.csv",
                                          snb_dyn + "comment_replyOf_comment_2_0.csv",
                                          snb_dyn + "comment_replyOf_comment_3_0.csv"};

  load_snb_data(graph, node_files, rship_files);

  result_set rs, expected;
  expected.data.push_back(
      {query_result("1649267442217"),
        query_result("maybe"),
        query_result("2012-01-10T06:31:18.533000"),
        query_result("15393162795439"),
        query_result("Lomana Trésor"),
        query_result("Kanam"),
        query_result("false")});

  expected.data.push_back(
      {query_result("1649267442213"),
        query_result("I see"),
        query_result("2012-01-10T14:57:10.420000"),
        query_result("19791209307382"),
        query_result("Amin"),
        query_result("Kamkar"),
        query_result("false")});

  ldbc_is_query_7(graph, rs);
  
  REQUIRE(rs == expected);

#ifdef USE_TX
  graph->commit_transaction();
#endif

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}

TEST_CASE("LDBC Interactive Insert Query 1", "[ldbc]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto graph = create_graph(pop);
#else
  auto graph = create_graph();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                          snb_sta + "place_0_0.csv", snb_sta + "place_1_0.csv",
                                          snb_sta + "place_2_0.csv", snb_sta + "place_3_0.csv",
                                          snb_sta + "tag_0_0.csv", snb_sta + "tag_1_0.csv",
                                          snb_sta + "tag_2_0.csv", snb_sta + "tag_3_0.csv",
                                          snb_sta + "organisation_0_0.csv", snb_sta + "organisation_1_0.csv",
                                          snb_sta + "organisation_2_0.csv", snb_sta + "organisation_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "person_isLocatedIn_place_0_0.csv",
                                          snb_dyn + "person_isLocatedIn_place_1_0.csv",
                                          snb_dyn + "person_isLocatedIn_place_2_0.csv",
                                          snb_dyn + "person_isLocatedIn_place_3_0.csv",
                                          snb_dyn + "person_hasInterest_tag_0_0.csv",
                                          snb_dyn + "person_hasInterest_tag_1_0.csv",
                                          snb_dyn + "person_hasInterest_tag_2_0.csv",
                                          snb_dyn + "person_hasInterest_tag_3_0.csv",
                                          snb_dyn + "person_studyAt_organisation_0_0.csv",
                                          snb_dyn + "person_studyAt_organisation_1_0.csv",
                                          snb_dyn + "person_studyAt_organisation_2_0.csv",
                                          snb_dyn + "person_studyAt_organisation_3_0.csv",
                                          snb_dyn + "person_workAt_organisation_0_0.csv",
                                          snb_dyn + "person_workAt_organisation_1_0.csv",
                                          snb_dyn + "person_workAt_organisation_2_0.csv",
                                          snb_dyn + "person_workAt_organisation_3_0.csv"};

  load_snb_data(graph, node_files, rship_files);

  result_set rs, expected;
  expected.append(
      {query_result("Person[35387]{birthday: 1981-Jan-21 00:00:00, browserUsed: \"Safari\", "
                    "creationDate: 2011-Jan-11 01:51:21.746000, email: \"\"new1@email1.com\", \"new@email2.com\"\", "
                    "firstName: \"New\", gender: \"female\", id: 9999999999999, language: \"\"zh\", \"en\"\", "
                    "lastName: \"Person\", locationIP: \"1.183.127.173\"}"),
      query_result("Place[10397]{id: 505, name: \"Artux\", type: \"city\", url: \"http://dbpedia.org/resource/Artux\"}"),
      query_result("::isLocatedIn[268661]{}"),
      query_result("Tag[11413]{id: 61, name: \"Kevin_Rudd\", url: \"http://dbpedia.org/resource/Kevin_Rudd\"}"),
      query_result("::hasInterest[268662]{}"),
      query_result("Organisation[29645]{id: 2213, name: \"Anhui_University_of_Science_and_Technology\", type: \"university\", "
                    "url: \"http://dbpedia.org/resource/Anhui_University_of_Science_and_Technology\"}"),
      query_result("::studyAt[268663]{classYear: 2001}"),
      query_result("Organisation[28347]{id: 915, name: \"Chang'an_Airlines\", type: \"company\", "
                    "url: \"http://dbpedia.org/resource/Chang'an_Airlines\"}"),
      query_result("::workAt[268664]{workFrom: 2001}") });
  
  ldbc_iu_query_1(graph, rs);
  
  REQUIRE(rs == expected);

#ifdef USE_TX
  graph->commit_transaction();
#endif

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}

TEST_CASE("LDBC Interactive Insert Query 2", "[ldbc]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto graph = create_graph(pop);
#else
  auto graph = create_graph();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                          snb_dyn + "post_0_0.csv", snb_dyn + "post_1_0.csv",
                                          snb_dyn + "post_2_0.csv", snb_dyn + "post_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "person_likes_post_0_0.csv",
                                          snb_dyn + "person_likes_post_1_0.csv",
                                          snb_dyn + "person_likes_post_2_0.csv",
                                          snb_dyn + "person_likes_post_3_0.csv"};

  load_snb_data(graph, node_files, rship_files);

  result_set rs, expected;
  expected.append(
      {query_result("Person[0]{birthday: 1989-Dec-03 00:00:00, browserUsed: \"Firefox\", "
                    "creationDate: 2010-Feb-14 15:32:10.447000, firstName: \"Mahinda\", "
                    "gender: \"male\", id: 933, lastName: \"Perera\", "
                    "locationIP: \"119.235.7.103\"}"),
        query_result("Post[532552]{browserUsed: \"Firefox\", content: \"About Alexander I of Russia, "
                      "lexander tried to introduce liberal reforms, while in the second half\", "
                      "creationDate: 2012-Aug-20 09:51:28.275000, id: 2061587303627, language: \"uz\", "
                      "length: 98, locationIP: \"14.205.203.83\"}"),
        query_result("::likes[751677]{creationDate: 2010-Feb-14 15:32:10.447000}")});
  
  ldbc_iu_query_2(graph, rs);
  
  REQUIRE(rs == expected);

#ifdef USE_TX
  graph->commit_transaction();
#endif

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}

TEST_CASE("LDBC Interactive Insert Query 3", "[ldbc]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto graph = create_graph(pop);
#else
  auto graph = create_graph();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                          snb_dyn + "comment_0_0.csv", snb_dyn + "comment_1_0.csv",
                                          snb_dyn + "comment_2_0.csv", snb_dyn + "comment_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "person_likes_comment_0_0.csv",
                                          snb_dyn + "person_likes_comment_1_0.csv",
                                          snb_dyn + "person_likes_comment_2_0.csv",
                                          snb_dyn + "person_likes_comment_3_0.csv"};

  load_snb_data(graph, node_files, rship_files);

  result_set rs, expected;
  expected.append(
      {query_result("Person[4807]{birthday: 1984-Oct-22 00:00:00, browserUsed: \"Chrome\", "
                    "creationDate: 2010-Jan-02 06:04:55.320000, firstName: \"Emperor of Brazil\", "
                    "gender: \"female\", id: 1564, lastName: \"Silva\", "
                    "locationIP: \"192.223.88.63\"}"),
        query_result("Comment[10108]{browserUsed: \"Firefox\", content: \"About Louis I of Hungary, "
                    "ittle lasting political results. Louis is theAbout Union of Sou\", "
                    "creationDate: 2012-Jan-19 11:39:51.385000, id: 1649267442250, length: 89, "
                    "locationIP: \"85.154.120.237\"}"),
        query_result("::likes[1438418]{creationDate: 2012-Jan-23 08:56:30.617000}")});
  
  ldbc_iu_query_3(graph, rs);
  
  REQUIRE(rs == expected);

#ifdef USE_TX
  graph->commit_transaction();
#endif

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}

TEST_CASE("LDBC Interactive Insert Query 4", "[ldbc]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto graph = create_graph(pop);
#else
  auto graph = create_graph();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  std::vector<std::string> node_files = {snb_dyn + "forum_0_0.csv", snb_dyn + "forum_1_0.csv",
                                          snb_dyn + "forum_2_0.csv", snb_dyn + "forum_3_0.csv",
                                          snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv", 
                                          snb_sta + "tag_0_0.csv", snb_sta + "tag_1_0.csv",
                                          snb_sta + "tag_2_0.csv", snb_sta + "tag_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "forum_hasModerator_person_0_0.csv",
                                            snb_dyn + "forum_hasModerator_person_1_0.csv",
                                            snb_dyn + "forum_hasModerator_person_2_0.csv",
                                            snb_dyn + "forum_hasModerator_person_3_0.csv",
                                            snb_dyn + "forum_hasTag_tag_0_0.csv",
                                            snb_dyn + "forum_hasTag_tag_1_0.csv",
                                            snb_dyn + "forum_hasTag_tag_2_0.csv",
                                            snb_dyn + "forum_hasTag_tag_3_0.csv"};

  load_snb_data(graph, node_files, rship_files);

  result_set rs, expected;
  expected.append(
      {query_result("Forum[116464]{creationDate: 2010-Jan-02 06:05:05.320000, id: 53975, title: "
                    "\"Wall of Emperor of Brazil Silva\"}"),
      query_result("Person[95299]{birthday: 1984-Oct-22 00:00:00, browserUsed: \"Chrome\", "
                    "creationDate: 2010-Jan-02 06:04:55.320000, firstName: \"Emperor of Brazil\", "
                    "gender: \"female\", id: 1564, lastName: \"Silva\", "
                    "locationIP: \"192.223.88.63\"}"), 
      query_result("::hasModerator[400258]{}"),
      query_result("Tag[100590]{id: 206, name: \"Charlemagne\", url: "
                    "\"http://dbpedia.org/resource/Charlemagne\"}"),
      query_result("::hasTag[400259]{}") });
  
  ldbc_iu_query_4(graph, rs);
  
  REQUIRE(rs == expected);

#ifdef USE_TX
  graph->commit_transaction();
#endif

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}

TEST_CASE("LDBC Interactive Insert Query 5", "[ldbc]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto graph = create_graph(pop);
#else
  auto graph = create_graph();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  std::vector<std::string> node_files = {snb_dyn + "forum_0_0.csv", snb_dyn + "forum_1_0.csv",
                                          snb_dyn + "forum_2_0.csv", snb_dyn + "forum_3_0.csv",
                                          snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "forum_hasMember_person_0_0.csv",
                                          snb_dyn + "forum_hasMember_person_1_0.csv",
                                          snb_dyn + "forum_hasMember_person_2_0.csv",
                                          snb_dyn + "forum_hasMember_person_3_0.csv"};

  load_snb_data(graph, node_files, rship_files);

  result_set rs, expected;
  expected.append(
      {query_result("Forum[33]{creationDate: 2010-Feb-15 00:46:27.657000, id: 37, title: \"Wall of Hồ Chí Do\"}"),
      query_result("Person[95299]{birthday: 1984-Oct-22 00:00:00, browserUsed: \"Chrome\", "
                    "creationDate: 2010-Jan-02 06:04:55.320000, firstName: \"Emperor of Brazil\", "
                    "gender: \"female\", id: 1564, lastName: \"Silva\", "
                    "locationIP: \"192.223.88.63\"}"),
      query_result("::hasMember[1611869]{creationDate: 2010-Feb-23 09:10:25.466000}")});
  
  ldbc_iu_query_5(graph, rs);
  
  REQUIRE(rs == expected);

#ifdef USE_TX
  graph->commit_transaction();
#endif

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}

TEST_CASE("LDBC Interactive Insert Query 6", "[ldbc]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto graph = create_graph(pop);
#else
  auto graph = create_graph();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  std::vector<std::string> node_files = {snb_dyn + "post_0_0.csv", snb_dyn + "post_1_0.csv",
                                          snb_dyn + "post_2_0.csv", snb_dyn + "post_3_0.csv",
                                          snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                          snb_dyn + "forum_0_0.csv", snb_dyn + "forum_1_0.csv",
                                          snb_dyn + "forum_2_0.csv", snb_dyn + "forum_3_0.csv",
                                          snb_sta + "place_0_0.csv", snb_sta + "place_1_0.csv",
                                          snb_sta + "place_2_0.csv", snb_sta + "place_3_0.csv",
                                          snb_sta + "tag_0_0.csv", snb_sta + "tag_1_0.csv",
                                          snb_sta + "tag_2_0.csv", snb_sta + "tag_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "post_hasCreator_person_0_0.csv",
                                            snb_dyn + "post_hasCreator_person_1_0.csv",
                                            snb_dyn + "post_hasCreator_person_2_0.csv",
                                            snb_dyn + "post_hasCreator_person_3_0.csv",
                                            snb_dyn + "forum_containerOf_post_0_0.csv",
                                            snb_dyn + "forum_containerOf_post_1_0.csv",
                                            snb_dyn + "forum_containerOf_post_2_0.csv",
                                            snb_dyn + "forum_containerOf_post_3_0.csv",
                                            snb_dyn + "post_isLocatedIn_place_0_0.csv",
                                            snb_dyn + "post_isLocatedIn_place_1_0.csv",
                                            snb_dyn + "post_isLocatedIn_place_2_0.csv",
                                            snb_dyn + "post_isLocatedIn_place_3_0.csv",
                                            snb_dyn + "post_hasTag_tag_0_0.csv",
                                            snb_dyn + "post_hasTag_tag_1_0.csv",
                                            snb_dyn + "post_hasTag_tag_2_0.csv",
                                            snb_dyn + "post_hasTag_tag_3_0.csv"};

  load_snb_data(graph, node_files, rship_files);

  result_set rs, expected;
  expected.append(
      {query_result("Post[1121529]{browser: \"Safari\", content: \"About Alexander I of "
                      "Russia,  (23 December  1777 – 1 December  1825), (Russian: "
                      "Александр Благословенный, Aleksandr Blagoslovennyi, meaning Alexander the Bless\", "
                      "creationDate: 2011-Sep-07 14:52:27.809000, id: 13439, imageFile: \"\", language: \"\"uz\"\", "
                      "length: 159, locationIP: \"46.19.159.176\"}"),
      query_result("Person[1013316]{birthday: 1987-Dec-01 00:00:00, browserUsed: \"Safari\", "
                    "creationDate: 2010-Aug-24 20:13:46.569000, firstName: \"Fritz\", "
                    "gender: \"female\", id: 6597069777240, lastName: \"Muller\", "
                    "locationIP: \"46.19.159.176\"}"),
      query_result("::hasCreator[3724073]{}"),
      query_result("Forum[1060792]{creationDate: 2010-Sep-21 16:25:35.425000, id: 549755871489, "
                    "title: \"Group for Alexander_I_of_Russia in Umeå\"}"),
      query_result("::containerOf[3724074]{}"),
      query_result("Place[1104039]{id: 50, name: \"Germany\", type: \"country\", "
                    "url: \"http://dbpedia.org/resource/Germany\"}"),
      query_result("::isLocatedn[3724075]{}"),
      query_result("Tag[1107128]{id: 1679, name: \"Alexander_I_of_Russia\", "
                    "url: \"http://dbpedia.org/resource/Alexander_I_of_Russia\"}"),
      query_result("::hasTag[3724076]{}")});
  
  ldbc_iu_query_6(graph, rs);
  
  REQUIRE(rs == expected);

#ifdef USE_TX
  graph->commit_transaction();
#endif

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}

TEST_CASE("LDBC Interactive Insert Query 7", "[ldbc]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto graph = create_graph(pop);
#else
  auto graph = create_graph();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  std::vector<std::string> node_files = {snb_dyn + "comment_0_0.csv", snb_dyn + "comment_1_0.csv",
                                          snb_dyn + "comment_2_0.csv", snb_dyn + "comment_3_0.csv",
                                          snb_dyn + "post_0_0.csv", snb_dyn + "post_1_0.csv",
                                          snb_dyn + "post_2_0.csv", snb_dyn + "post_3_0.csv",
                                          snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                          snb_sta + "place_0_0.csv", snb_sta + "place_1_0.csv",
                                          snb_sta + "place_2_0.csv", snb_sta + "place_3_0.csv",
                                          snb_sta + "tag_0_0.csv", snb_sta + "tag_1_0.csv",
                                          snb_sta + "tag_2_0.csv", snb_sta + "tag_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "comment_hasCreator_person_0_0.csv",
                                            snb_dyn + "comment_hasCreator_person_1_0.csv",
                                            snb_dyn + "comment_hasCreator_person_2_0.csv",
                                            snb_dyn + "comment_hasCreator_person_3_0.csv",
                                            snb_dyn + "comment_replyOf_post_0_0.csv",
                                            snb_dyn + "comment_replyOf_post_1_0.csv",
                                            snb_dyn + "comment_replyOf_post_2_0.csv",
                                            snb_dyn + "comment_replyOf_post_3_0.csv",
                                            snb_dyn + "comment_isLocatedIn_place_0_0.csv",
                                            snb_dyn + "comment_isLocatedIn_place_1_0.csv",
                                            snb_dyn + "comment_isLocatedIn_place_2_0.csv",
                                            snb_dyn + "comment_isLocatedIn_place_3_0.csv",
                                            snb_dyn + "comment_hasTag_tag_0_0.csv",
                                            snb_dyn + "comment_hasTag_tag_1_0.csv",
                                            snb_dyn + "comment_hasTag_tag_2_0.csv",
                                            snb_dyn + "comment_hasTag_tag_3_0.csv"};

  load_snb_data(graph, node_files, rship_files);

result_set rs, expected;
  expected.append(
      {query_result("Comment[3083206]{browser: \"Chrome\", content: \"fine\", creationDate: "
                    "2012-Jan-09 11:49:15.991000, "
                      "id: 442214, length: 4, locationIP: \"91.149.169.27\"}"),
      query_result("Person[3059734]{birthday: 1990-Jan-03 00:00:00, browserUsed: \"Chrome\", "
                    "creationDate: 2010-Dec-25 08:07:55.284000, "
                    "firstName: \"Ivan Ignatyevich\", gender: \"male\", id: 10995116283243, "
                    "lastName: \"Aleksandrov\", locationIP: \"91.149.169.27\"}"),
      query_result("::hasCreator[7814151]{}"),
      query_result("Post[2052494]{browserUsed: \"Internet Explorer\", content: \"About Louis I of Hungary, "
                    "dwig der Große, Bulgarian: Лудвиг I, Serbian: Лајош I Анжујски, Czech: Ludvík I. "
                    "Veliký, Li\", creationDate: 2012-Jan-09 07:50:59.110000, id: 1649267442210, "
                    "language: \"tk\", length: 117, locationIP: \"192.147.218.174\"}"),
      query_result("::replyOf[7814152]{}"),
      query_result("Place[3065729]{id: 63, name: \"Belarus\", type: \"country\", url: "
                    "\"http://dbpedia.org/resource/Belarus\"}"),
      query_result("::isLocatedn[7814153]{}"),
      query_result("Tag[3068805]{id: 1679, name: \"Alexander_I_of_Russia\", "
                    "url: \"http://dbpedia.org/resource/Alexander_I_of_Russia\"}"),
      query_result("::hasTag[7814154]{}")});
  
  ldbc_iu_query_7(graph, rs);
  
  REQUIRE(rs == expected);

#ifdef USE_TX
  graph->commit_transaction();
#endif

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}

TEST_CASE("LDBC Interactive Insert Query 8", "[ldbc]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto graph = create_graph(pop);
#else
  auto graph = create_graph();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "person_knows_person_0_0.csv",
                                          snb_dyn + "person_knows_person_1_0.csv",
                                          snb_dyn + "person_knows_person_2_0.csv",
                                          snb_dyn + "person_knows_person_3_0.csv"};

  load_snb_data(graph, node_files, rship_files);

  result_set rs, expected;
  expected.append(
      {query_result("Person[2]{birthday: 1988-Oct-14 00:00:00, browserUsed: \"Internet Explorer\", "
                    "creationDate: 2010-Feb-15 00:46:17.657000, firstName: \"Hồ Chí\", "
                    "gender: \"male\", id: 4194, lastName: \"Do\", "
                    "locationIP: \"103.2.223.188\"}"),
      query_result("Person[4807]{birthday: 1984-Oct-22 00:00:00, browserUsed: \"Chrome\", "
                    "creationDate: 2010-Jan-02 06:04:55.320000, firstName: \"Emperor of Brazil\", "
                    "gender: \"female\", id: 1564, lastName: \"Silva\", "
                    "locationIP: \"192.223.88.63\"}"),
        query_result("::KNOWS[180623]{creationDate: 2010-Feb-23 09:10:15.466000}")});
  
  ldbc_iu_query_8(graph, rs);
  
  REQUIRE(rs == expected);

#ifdef USE_TX
  graph->commit_transaction();
#endif

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}*/

/*
class root {
public:
	graph_db_ptr graph_ptr;
};

TEST_CASE("LDBC Graph Data", "[ldbc]") {
#ifdef USE_PMDK
nvm::pool<root> pop;
p_ptr<root> root_ptr;

bool new_pool = false;
if (access(test_path.c_str(), F_OK) != 0) {
  pop = nvm::pool<root>::create (test_path, "", PMEMOBJ_POOL_SIZE); 
  std::cout << "pool created \n";
  new_pool = true;
} else {
  pop = nvm::pool<root>::open (test_path, "");
  std::cout << "pool opened \n";
}

root_ptr = pop.root ();
PMEMoid *root_ptr_oid = root_ptr.raw_ptr();
root *r = (root *) pmemobj_direct(*root_ptr_oid);
if(new_pool)
  root_ptr->graph_ptr = create_graph(pop);
//auto graph = r->graph_ptr;
#else
  auto graph = create_graph();
#endif

#ifdef USE_TX
  //auto tx = graph->begin_transaction();
  auto tx = root_ptr->graph_ptr->begin_transaction();
#endif

  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                          snb_sta + "place_0_0.csv", snb_sta + "place_1_0.csv",
                                          snb_sta + "place_2_0.csv", snb_sta + "place_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "person_isLocatedIn_place_0_0.csv",
                                          snb_dyn + "person_isLocatedIn_place_1_0.csv",
                                          snb_dyn + "person_isLocatedIn_place_2_0.csv",
                                          snb_dyn + "person_isLocatedIn_place_3_0.csv"};

  //load_snb_data(graph, node_files, rship_files);

  result_set rs, expected;
  expected.data.push_back(
      {query_result("Mahinda"), query_result("Perera"),
        query_result("1989-12-03"), query_result("119.235.7.103"),
        query_result("Firefox"), query_result("1353"), query_result("male"),
        query_result("2010-02-14T15:32:10.447000")});
#ifdef USE_PMDK
  ldbc_is_query_1(root_ptr->graph_ptr, rs);
#else
ldbc_is_query_1(graph, rs);
#endif

  REQUIRE(rs == expected);

#ifdef USE_TX
  //graph->commit_transaction();
  root_ptr->graph_ptr->commit_transaction();
#endif

#ifdef USE_PMDK
  //nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  //remove(test_path.c_str());
#endif
}
*/

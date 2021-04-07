## BI 1
Given a date, find all Messages created before that date. Group them by a 3-level grouping:
1. by year of creation
2. for each year, group into Message types: is Comment or not
3. for each year-type group, split into four groups based on length of their content
   * 0: 0 <= length < 40 (short)
   * 1: 40 <= length < 80 (one liner) 
   * 2: 80 <= length < 160 (tweet)
   * 3: 160 <= length (long)

## BI 2
Select all Messages created in the range of [startDate, endDate] by Persons located in country1 or country2. Select the creator Persons and the Tags of these Messages. Split these Persons, Tags and Messages into a 5-level grouping:
1. name of country of Person,
2. month the Message was created,
3. gender of Person,
4. age group of Person, defined as years between person’s birthday and end of simulation
(2013-01-01), divided by 5, rounded down (partial years do not count),
5. name of tag attached to Message.
Consider only those groups where number of Messages is greater than 100.

## BI 3
Find the Tags that were used in Messages during the given month of the given year and the Tags that were used during the next month.
For the Tags and for both months, compute the count of Messages.

## BI 4
Given a TagClass and a Country, find all the Forums created in the given Country, containing at least one Post with Tags belonging directly to the given TagClass.
The location of a Forum is identified by the location of the Forum’s moderator.

## BI 5
Find the most popular Forums for a given Country, where the popularity of a Forum is measured by the number of members that Forum has from the given Country.
Calculate the top 100 most popular Forums. In case of a tie, the forum(s) with the smaller id value(s) should be selected.
For each member Person of the 100 most popular Forums, count the number of Posts (postCount) they made in any of those (most popular) Forums. Also include those member Persons who have not posted any messages (have a postCount of 0).

## BI 6
Get each Person (person) who has created a Message (message) with a given Tag (direct relation, not transitive). Considering only these messages, for each Person node:
* Count its messages (messageCount).
* Count likes (likeCount) to its messages.
* Count Comments (replyCount) in reply to it messages.
The score is calculated according to the following formula: 1 * messageCount + 2 * replyCount + 10 * likeCount.

## BI 7
Given a Tag, find all Persons (person) that ever created a Message (message1) with the given Tag. For each of these Persons (person) compute their “authority score” as follows:
* The “authority score” is the sum of “popularity scores” of the Persons (person2) that liked any of that Person’s Messages (message2) with the given Tag.
* A Person’s (person2) “popularity score” is defined as the total number of likes on all of their Messages (message3).

## BI 8
Find all Messages that have a given Tag. Find the related Tags attached to (direct) reply Comments of these Messages, but only of those reply Comments that do not have the given Tag.
Group the Tags by name, and get the count of replies in each group.

## BI 9
Given two TagClasses (tagClass1 and tagClass2), find Forums that contain
* at least one Post (post1) with a Tag with a (direct) type of tagClass1 and
* at least one Post (post2) with a Tag with a (direct) type of tagClass2.
The post1 and post2 nodes may be the same Post.
Consider the Forums with a number of members greater than a given threshold. For every such Forum, count the number of post1 nodes (count1) and the number of post2 nodes (count2).

## BI 10
Given a Tag, find all Persons that are interested in the Tag and/or have written a Message (Post or Comment) with a creationDate after a given date and that has a given Tag. For each Person, compute the score as the sum of the following two aspects:
* 100, if the Person has this Tag as their interest, or 0 otherwise 
* number of Messages by this Person with the given Tag
Also, for each Person, compute the sum of the score of the Person’s friends (friendsScore).

## BI 11
Find those Persons of a given Country that replied to any Message, such that the reply does not have any Tag in common with the Message (only direct replies are considered, transitive ones are not). Consider only those replies that do no contain any word from a given blacklist. For each Person and valid reply, retrieve the Tags associated with the reply, and retrieve the number of likes on the reply.
The detailed conditions for checking blacklisted words are currently as follows. Words do not have to stand separately, i.e. if the word “Green” is blacklisted, “South-Greenland” cannot be in- cluded in the results. Also, comparison should be done in a case-sensitive way. These conditions are preliminary and might be changed in later versions of the benchmark.

## BI 12
Find all Messages created after a given date (exclusive), that received more than a given number of likes (likeThreshold).

## BI 13
Find all Messages in a given Country, as well as their Tags.
Group Messages by creation year and month. For each group, find the 5 most popular Tags, where popularity is the number of Messages (from within the same group) where the Tag appears. Note: even if there are no Tags for Messages in a given year and month, the result should include the year and month with an empty popularTags list.

## BI 14
For each Person,count the number of Posts they created in the time interval [startDate, endDate] (equivalent to the number of threads they initiated) and the number of Messages in each of their (transitive) reply trees, including the root Post of each tree. When calculating Message counts only consider messages created within the given time interval.
Return each Person, number of Posts they created, and the count of all Messages that appeared in the reply trees (including the Post at the root of tree) they created.

## BI 15
Given a Country country, determine the “social normal”, i.e. the floor of average number of friends that Persons of country have in country.
Then, find all Persons in country, whose number of friends in country equals the social normal value.

## BI 16
Given a Person, find all other Persons that live in a given Country and are connected to given Person by a transitive trail with length in range [minPathDistance, maxPathDistance] through the knows relation.
In the trail, an edge can be only traversed once while nodes can be traversed multiple times (as opposed to a path which allows repetitions of both nodes and edges).
For each of these Persons, retrieve all of their Messages that contain at least one Tag belonging to a given TagClass (direct relation not transitive). For each Message, retrieve all of its Tags. Group the results by Persons and Tags, then count the Messages by a certain Person having a certain Tag.
(Note: it is not yet decided whether a Person connected to the start Person on a trail with a length smaller than minPathDistance, but also on a trail with the length in [minPathDistance, maxPathDistance] should be included. The current reference implementations allow such Persons, but this might be subject to change in the future.)

## BI 17
For a given country, count all the distinct triples of Persons such that:
* a is friend of b,
* b is friend of c, 
* c is friend of a.
Distinct means that given a triple t1 in the result set R of all qualified triples, there is no triple t2 in R such that t1 and t2 have the same set of elements.

## BI 18
For each Person, count the number of Messages they made (messageCount). Only count Messages with the following attributes:
* Its content is not empty (and consequently, imageFile empty for Posts).
* Its length is below the lengthThreshold (exclusive, equality is not allowed).
* Its creationDate is after date (exclusive, equality is not allowed).
* It is written in any of the given languages.
  * The language of a Post is defined by its language attribute.
  * The language of a Comment is that of the Post that initiates the thread where the
Comment replies to.
The Post and Comments in the reply tree’s path (from the Message to the Post) do not have
to satisfy the constraints for content, length and creationDate.
For each messageCount value, count the number of Persons with exactly messageCount Messages (with the required attributes).

## BI 19
For all the Persons (person) born after a certain date, find all the strangers they interacted with, where strangers are Persons that do not know person. There is no restriction on the date that strangers were born. (Of course, person and stranger are required to be two different Persons.) Consider only strangers that are
* members of Forums tagged with a Tag with a (direct) type of tagClass1 and 
* members of Forums tagged with a Tag with a (direct) type of tagClass2.
The Tags may be attached to the same Forum or they may be attached to different Forums. Interaction is defined as follows: the person has replied to a Message by the stranger B (the reply might be a transitive one).
For each person, count the number of strangers they interacted with (strangerCount) and total number of times they interacted with them (interactionCount).

## BI 20
For all given TagClasses, count number of Messages that have a Tag that belongs to that TagClass or any of its children (all descendants through a transitive relation).

## BI 21
Find zombies within the given country, and return their zombie scores. A zombie is a Person created before the given endDate, which has created an average of [0, 1) Messages per month, during the time range between profile’s creationDate and the given endDate. The number of months spans the time range from the creationDate of the profile to the endDate with partial months on both end counting as one month (e.g. a creationDate of Jan 31 and an endDate of Mar 1 result in 3 months).
For each zombie, calculate the following:
* zombieLikeCount: the number of likes received from other zombies.
* totalLikeCount: the total number of likes received.
* zombieScore: zombieLikeCount / totalLikeCount. If the value of totalLikeCount is 0, the
zombieScore of the zombie should be 0.0.
For both zombieLikeCount and totalLikeCount, only consider likes received from profiles that were
created before the given endDate.

## BI 22
Consider all pairs of people (person1, person2) such that one is located in a City of Country country1 and the other is located in a City of Country country2. For each City of Country country1, return the highest scoring pair. The score of a pair is defined as the sum of the subscores awarded for the following kinds of interaction. The initial value is score = 0.
1. person1 has created a reply Comment to at least one Message by person2: score += 4
2. person1 has created at least one Message that person2 has created a reply Comment to: score
+= 1
3. person1 and person2 know each other: score += 15
4. person1 liked at least one Message by person2: score += 10
5. person1 has created at least one Message that was liked by person2: score += 1
Consequently, the maximum score a pair can obtain is: 4 + 1 + 15 + 10 + 1 = 31. To break ties, order by (1) person1.id ascending and (2) person2.id ascending.

## BI 23
Count the Messages of all residents of a given Country (home), where the message was written abroad. Group the messages by month and destination.
A Message was written abroad if it is located in a Country (destination) different than home.

## BI 24
Find all Messages tagged with a Tag that has the (direct) type of the given tagClass. Count all Messages and their likes grouped by Continent, year, and month.

## BI 25
Given two Persons, find all (unweighted) shortest paths between these two Persons, in the sub- graph induced by the knows relationship.
Then, for each path calculate a weight. The nodes in the path are Persons, and the weight of a path is the sum of weights between every pair of consecutive Person nodes in the path.
The weight for a pair of Persons is calculated based on their interactions:
* Every direct reply (by one of the Persons) to a Post (by the other Person) contributes 1.0. 
* Every direct reply (by one of the Persons) to a Comment (by the other Person) contributes 0.5.
Only consider Messages that were created in a Forum that was created within the timeframe [startDate, endDate]. Note that for Comments, the containing Forum is that of the Post that the comment (transitively) replies to.
Return all paths with the Person ids ordered by their weights descending.


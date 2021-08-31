## BI 1. Posting summary
Given a date, find all Messages created before that date. Group them by a 3-level grouping:
1. by year of creation
2. for each year, group into Message types: is Comment or not
3. for each year-type group, split into four groups based on length of their content
   * 0: 0 <= length < 40 (short)
   * 1: 40 <= length < 80 (one liner) 
   * 2: 80 <= length < 160 (tweet)
   * 3: 160 <= length (long)

## BI 2. Tag evolution
Find the Tags that were used in Messages during in the 100-day period starting at date and compare
it with the 100-day period that follows. For the Tags and for both months, compute the count of
Messages.

## BI 3. Popular topics in a country
Given a TagClass and a Country, find all the Forums created in the given Country, containing at least
one Message with Tags belonging directly to the given TagClass, and count the Messages by the
Person who created it and the Forum where they were posted.
The location of a Forum is identified by the location of the Forum’s moderator.

## BI 4. Top message creators by country
Find the most popular Forums by Country, where the popularity of a Forum is measured by the
number of members that Forum has from a given Country.
Calculate the top 100 most popular Forums. In case of a tie, the Forum(s) with the smaller id
value(s) should be selected.
For each member Person of the 100 most popular Forums, count the number of Messages
(messageCount) they made in any of those (most popular) Forums. Also include those member
Persons who have not posted any Messages (have a messageCount of 0).

## BI 5. Most active posters of a given topic
Get each Person (person) who has created a Message (message) with a given Tag (direct relation,
not transitive). Considering only these messages, for each Person node:
  * Count its messages (messageCount).
  * Count likes (likeCount) to its messages.
  * Count Comments (replyCount) in reply to it messages.

The score is calculated according to the following formula: 1 × messageCount + 2 × replyCount +
10 × likeCount.

## BI 6. Most authoritative users on a given topic
Given a Tag (tag), find all Persons (person) that ever created a Message with the Tag. For each of
these Persons (person) compute their “authority score” as follows:
  * The “authority score” is the sum of “popularity scores” of the Persons (p2) that liked any of that Person’s Messages with the given Tag (same criterion as for message1).
  * A Person’s (p2) “popularity score” is defined as the total number of likes on all of their Messages (message2).

## BI 7. Related topics
Find all Messages that have a given Tag. Find the related Tags attached to (direct) reply Comments
of these Messages, but only of those reply Comments that do not have the given Tag.
Group the Tags by name, and get the count of replies in each group.

## BI 8. Central person for a tag
Given a Tag, find all Persons that are interested in the Tag and/or have written a Message (Post
or Comment) with a creationDate after a given date and that has a given Tag. For each Person,
compute the score as the sum of the following two aspects:
  * 100, if the Person has this Tag as their interest, or 0 otherwise
  * number of Messages by this Person with the given Tag

## BI 9. Top thread initiators
For each Person, count the number of Posts they created in the time interval [startDate, endDate]
(equivalent to the number of threads they initiated) and the number of Messages in each of their
(transitive) reply trees, including the root Post of each tree. When calculating Message counts
only consider messages created within the given time interval.
Return each Person, number of Posts they created, and the count of all Messages that appeared in
the reply trees (including the Post at the root of tree) they created.

## BI 10. Experts in social circle
Given a Person, find all other Persons that live in a given Country and are connected to given Person
by a shortest path with length in range [minPathDistance, maxPathDistance] through the knows
relation.
For each of these Persons, retrieve all of their Messages that contain at least one Tag belonging to
a given TagClass (direct relation not transitive). For each Message, retrieve all of its Tags.
Group the results by Persons and Tags, then count the Messages by a certain Person having a certain
Tag.

## BI 11. Friend triangles
For a given country, count all the distinct triples of Persons such that:
  * a is friend of b,
  * b is friend of c,
  * c is friend of a,

and these friendships were created after a given startDate.
Distinct means that given a triple t1 in the result set R of all qualified triples, there is no triple
t2 in R such that t1 and t2 have the same set of elements.

## BI 12. How many persons have a given number of messages
For each Person, count the number of Messages they made (messageCount). Only count Messages
with the following attributes:
  * Its content is not empty (and consequently, the imageFile attribute is empty for Posts).
  * Its length is below the lengthThreshold (exclusive, equality is not allowed).
  * Its creationDate is after date (exclusive, equality is not allowed).
  * It is written in any of the given languages.
    - The language of a Post is defined by its language attribute.
    - The language of a Comment is that of the Post that initiates the thread where the Comment replies to.

The Post and Comments in the reply tree’s path (from the Message to the Post) do not have to 
satisfy the constraints for content, length and creationDate. For each messageCount value, 
count the number of Persons with exactly (with the required attributes).

## BI 13. Zombies in a country
Find zombies within the given country, and return their zombie scores. A zombie is a Person
created before the given endDate, which has created an average of [0, 1) Messages per month,
during the time range between profile’s creationDate and the given endDate. The number of
months spans the time range from the creationDate of the profile to the endDate with partial
months on both end counting as one month (e.g. a creationDate of Jan 31 and an endDate of Mar
1 result in 3 months).
For each zombie , calculate the following:
  * zombieLikeCount: the number of likes received from other zombies.
  * totalLikeCount: the total number of likes received.
  * zombieScore: zombieLikeCount / totalLikeCount. If the value of totalLikeCount is 0, the zombieScore of the zombie should be 0.0.

## BI 14. International dialog
Consider all pairs of people (person1, person2) such that one is located in a City of Country coun-
try1 and the other is located in a City of Country country2 . For each City of Country country1 , return
the highest scoring pair. The score of a pair is defined as the sum of the subscores awarded for
the following kinds of interaction. The initial value is score = 0 .
  1. person1 has created a reply Comment to at least one Message by person2: score += 4
  2. person1 has created at least one Message that person2 has created a reply Comment to: score += 1
  3. person1 and person2 know each other: score += 15
  4. person1 liked at least one Message by person2 : score += 10
  5. person1 has created at least one Message that was liked by person2: score += 1

Consequently, the maximum score a pair can obtain is: 4 + 1 + 15 + 10 + 1 = 31.
To break ties, order by (1) person1.id ascending and (2) person2.id ascending.

## BI 15. Trusted connection paths through forums created in a given timeframe
Given two Persons, find all (unweighted) shortest paths between these two Persons, in the subgraph
induced by the knows relationship.
Then, for each path calculate a weight. The nodes in the path are Persons, and the weight of a
path is the sum of weights between every pair of consecutive Person nodes in the path.
The weight for a pair of Persons is calculated based on their interactions:
  * Every direct reply (by one of the Persons) to a Post (by the other Person) contributes 1.0.
  * Every direct reply (by one of the Persons) to a Comment (by the other Person) contributes 0.5.

Only consider Messages that were created in a Forum that was created within the timeframe (inter-
val) [startDate, endDate] . Note that for Comments, the containing Forum is that of the Post that
the comment (transitively) replies to. Also note that interactions are counted both ways.
Return all paths with the Person IDs ordered by their weights descending.

## BI 16. Fake news detection
Given two Tag/date pairs (tagA/dateA and tagB/dateB), for each pair tagX/dateX:
  * Create an induced subgraph between Persons where for each pair of Persons person1/person2, 
  both have created a Message on the day of dateX with Tag tagX.
  * In the induced subgraph, only keep pairs of Persons who have at most maxKnowsLimit friends 
  (in the induced subgraph).
  * For these Persons, count the number of Messages created on dateX with Tag tagX.

Return Persons who had at least one messages for both tagA/dateA and tagB/dateB ranked by their
total number of messages (descending).

## BI 17. Information propagation analysis
This query aims to identify instances of “information propagation” when a Person (person1) submits 
a Message (message1) with a given Tag (tag) to a Forum (forum1). This is read by other members
of forum1, Persons person2 and person3. Some time later (specified by the delta parameter), these
persons have a discussion with the same tag in a different Forum (forum2) where person1 is not a
member. The discussion consists of a Message (message2) by person2 and a direct reply Comment
(comment) by person3.
Return IDs of person1 with the number of interactions their Messages (might have) caused.

## BI 18. Friend recommendation
For a given Person (person1) and a Tag (tag), recommend new friends (person2) who
  * do not yet know person1
  * have many mutual friends with person1
  * are interested in tag.

Rank Persons person2 based on the number of mutual friends.

## BI 19. Interaction path between cities
Given two Cities city1, city2, find Persons person1, person2 living in these Cities (respectively) with
the shortest interaction path between them. If there are multiple pairs of people with shortest
paths having the same total weight, return all of them.
The shortest path is computed using a weight between two Persons defined as the reciprocal of
the number of interactions (direct reply Comments to a Message by the other Person). Therefore,
more interactions imply a smaller weight.
Note: Interactions are counted both ways, i.e. if Alice writes 2 reply Comments to Bob’s Messages
and Bob writes 3 reply Comments to Alice’s Messages , their total number of interactions is 5.

## BI 20. Recruitment
Given a Company company and a Person person2 (who is known to be working at another Company),
find a Person (person1) working the in the company who have the top-20 shortest path to person2
through people who have studied together. On this path, we only consider edges between Persons
who know each other and attended the same university and set the weight of the edge to the
absolute difference between the year of enrolment plus 1 (studyAt.classYear + 1).
If there are multiple Person person1 nodes with the same shortest path, return all of them.
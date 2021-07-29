## IS 1
Given a start Person, retrieve their first name, last name, birthday, IP address, browser, and city of residence.

## IS 2
Given a start Person, retrieve the last 10 Messages created by that user. For each Message, return that Message, the original Post in its conversation, and the author of that Post. If any of the Messages is a Post, then the original Post will be the same Message, i.e. that Message will appear twice in that result.

## IS 3
Given a start Person, retrieve all of their friends, and the date at which they became friends.

## IS 4
Given a Message, retrieve its content and creation date.

## IS 5
Given a Message, retrieve its author.

## IS 6
Given a Message, retrieve the Forum that contains it and the Person that moderates that Forum. Since Comments are not directly contained in Forums, for Comments, return the Forum containing the original Post in the thread which the Comment is replying to.

## IS 7
Given a Message, retrieve the (1-hop) Comments that reply to it.
In addition, return a boolean flag knows indicating if the author of the reply knows the author of the original message. If author is same as original author, return false for knows flag.

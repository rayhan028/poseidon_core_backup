# Running LDBC with Poseidon

1. Download GTPC from https://ldbcouncil.org/data-sets-surf-repository/snb-interactive-v1-datagen-v035.html

2. Extract and copy content of static/ and dynamic/ into single directory, e.g., data/ 

3. Import the data in Poseidon

```
${POSEIDON_HOME}/build/pcli -b 100000 --pool ldbc --db testdb --delimiter '|' \
    --import-path data/ \
    --import nodes:Comment:comment_0_0.csv \
    --import nodes:Forum:forum_0_0.csv \
    --import nodes:Person:person_0_0.csv \
    --import nodes:Post:post_0_0.csv \
    --import nodes:Organisation:organisation_0_0.csv \
    --import nodes:Place:place_0_0.csv \
    --import nodes:Tag:tag_0_0.csv \
    --import nodes:TagClass:tagclass_0_0.csv \
    --import relationships:hasCreator:comment_hasCreator_person_0_0.csv \
    --import relationships:hasTag:comment_hasTag_tag_0_0.csv \
    --import relationships:isLocatedIn:comment_isLocatedIn_place_0_0.csv \
    --import relationships:replyOf:comment_replyOf_comment_0_0.csv \
    --import relationships:replyOf:comment_replyOf_post_0_0.csv \
    --import relationships:containerOf:forum_containerOf_post_0_0.csv \
    --import relationships:hasMember:forum_hasMember_person_0_0.csv \
    --import relationships:hasModerator:forum_hasModerator_person_0_0.csv \
    --import relationships:hasTag:forum_hasTag_tag_0_0.csv \
    --import relationships:hasInterest:person_hasInterest_tag_0_0.csv \
    --import relationships:isLocatedIn:person_isLocatedIn_place_0_0.csv \
    --import relationships:knows:person_knows_person_0_0.csv \
    --import relationships:likes:person_likes_comment_0_0.csv \
    --import relationships:likes:person_likes_post_0_0.csv \
    --import relationships:studyAt:person_studyAt_organisation_0_0.csv \
    --import relationships:workAt:person_workAt_organisation_0_0.csv \
    --import relationships:hasCreator:post_hasCreator_person_0_0.csv \
    --import relationships:hasTag:post_hasTag_tag_0_0.csv \
    --import relationships:isLocatedIn:post_isLocatedIn_place_0_0.csv \
    --import relationships:isLocatedIn:organisation_isLocatedIn_place_0_0.csv \
    --import relationships:isPartOf:place_isPartOf_place_0_0.csv \
    --import relationships:hasType:tag_hasType_tagclass_0_0.csv \
    --import relationships:isSubclassOf:tagclass_isSubclassOf_tagclass_0_0.csv \
    --typespec types.mapping
```

This creates a database `testdb` in the pool `ldbc` in the current directory.

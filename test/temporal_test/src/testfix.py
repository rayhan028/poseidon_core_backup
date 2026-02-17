import csv
import os

def fix_all_edge_files(base_path):
    # List of edge files to fix
    edge_files = [
        "person_knows_person_0_0.csv",
        "comment_hasCreator_person_0_0.csv", 
        "post_hasCreator_person_0_0.csv",
        "forum_hasMember_person_0_0.csv",
        "person_likes_comment_0_0.csv",
        "person_likes_post_0_0.csv",
        "forum_hasModerator_person_0_0.csv",
        "person_workAt_organisation_0_0.csv",
        "person_studyAt_organisation_0_0.csv",
        "person_isLocatedIn_place_0_0.csv",
        "comment_isLocatedIn_place_0_0.csv",
        "post_isLocatedIn_place_0_0.csv",
        "organisation_isLocatedIn_place_0_0.csv",
        "person_hasInterest_tag_0_0.csv",
        "comment_hasTag_tag_0_0.csv",
        "post_hasTag_tag_0_0.csv",
        "forum_hasTag_tag_0_0.csv",
        "person_speaks_language_0_0.csv",
        "person_email_emailaddress_0_0.csv",
        "tag_hasType_tagclass_0_0.csv",
        "tagclass_isSubclassOf_tagclass_0_0.csv",
        "place_isPartOf_place_0_0.csv",
        "comment_replyOf_comment_0_0.csv",
        "comment_replyOf_post_0_0.csv",
        "forum_containerOf_post_0_0.csv"
    ]
    
    for filename in edge_files:
        input_path = os.path.join(base_path, "dynamic", filename)
        output_path = os.path.join(base_path, "dynamic", filename.replace(".csv", "_FIXED.csv"))
        
        if not os.path.exists(input_path):
            # Check static directory
            input_path = os.path.join(base_path, "static", filename)
            output_path = os.path.join(base_path, "static", filename.replace(".csv", "_FIXED.csv"))
        
        if not os.path.exists(input_path):
            print(f"File not found: {filename}")
            continue
        
        print(f"Fixing {filename}...")
        
        with open(input_path, 'r') as f_in, open(output_path, 'w') as f_out:
            reader = csv.reader(f_in, delimiter='|')
            writer = csv.writer(f_out, delimiter='|')
            
            header = next(reader)
            writer.writerow(header)
            
            fixed_count = 0
            for row in reader:
                if len(row) >= 4:
                    new_row = []
                    # Fix Person IDs (HUGE → small valid IDs 1-9892)
                    for i, val in enumerate(row):
                        if "Person.id" in header[i] or "person" in header[i].lower():
                            # Convert huge ID to valid range
                            try:
                                num = int(val)
                                if num > 1000000:  # If it's huge
                                    fixed = (num % 9892) + 1
                                    new_row.append(str(fixed))
                                else:
                                    new_row.append(val)
                            except:
                                new_row.append(val)
                        else:
                            new_row.append(val)
                    
                    writer.writerow(new_row)
                    fixed_count += 1
            
            print(f"  Fixed {fixed_count} edges")

# Run it
fix_all_edge_files("/home/rayhan/poseidon_core/Temporal-data-LDBC/sf1_enhanced")
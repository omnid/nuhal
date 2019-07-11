#!/usr/bin/env bash
# Usage: make_git_hash_h.sh [OUTPUT]
# Creates git_hash.h, a file containing git hashes for the project
# This will provide definitions for
# 1. The hash of the current HEAD
# 2. Whether the repository is clean or dirty
# 3. The hash of each individual board's project and non_ros/all.
#    which projects to use are specified as an array at the beginning of this file
# 1 and 2 will let you be able to (as close as possible) checkout the git revision
# that corresponds to code running on a robot.  Step 3 enables our build/loading
# infrastructure to conservatively determine when the firmware must be updated
#
# With 0 arguments, the header is written to stdout
# If OUTPUT is specified then
# the generated header is compared to the OUTPUT file
# and the data is written only if they differ

#### SETUP indiividual project SHA1 generation
# dependencies are files and directories that are included in every project's SHA1 hash
# these will be included in the SHA1 of every project
# paths are relative to non_ros/
dependencies="CMakeLists.txt all tiva/driverlib tiva/tlib"
# This array lists project names, relative to non_ros/tiva
# each individual project gets its own hash
projects="delta gimbal joint omni wheel"


# make current working directory the location of this script
mypath=$(readlink -f $(dirname $0))
cd $mypath

# find the top-level directory for the project
git_dir=$(git rev-parse --show-toplevel)/

# get the sha1 of the current HEAD
head_sha1=$(git rev-parse HEAD)

# get if the repository is dirty
git diff --quiet HEAD
dirty=$?

# create a temporary index and add all unstaged changes to this index
temp_index=$(mktemp)
cp $git_dir/.git/index $temp_index

# make git use the temporary index file 
export GIT_INDEX_FILE=$temp_index
cd $git_dir
git add -u


temp_out=$(mktemp)
cat > "$temp_out" <<EOF
// Automatically generated by non_ros/all/common/make_git_hash_h.sh
// DO NOT MODIFY.  To add new projects, see make_git_hash_h.sh
#define GIT_HASH_HEAD $head_sha1
#define GIT_HASH_DIRTY $dirty
EOF


cd $git_dir/non_ros
for proj in $projects
do
    # get the sha1 hash of the directories that affect embedded code
    tiva_hash=$(git ls-files -s $dependencies tiva/$proj | git hash-object --stdin)
    # convert project name to upper case
    echo "#define GIT_HASH_${proj^^} $tiva_hash" >> $temp_out
done


cd $mypath
if [ -z "$1" ]
then
   cat "$temp_out" > /dev/stdout
else
    mkdir -p $(dirname "$1")
    [ -f "$1" ] || touch "$1"
    # copy temporary to output if they differ
    cmp -s  "$temp_out" "$1" || mv "$temp_out" "$1"
fi
rm -f "$temp_out"
rm -f "$temp_index"

import sys
import re

from pyspark import SparkContext,SparkConf

def splitDocument(document):
    """Returns a list of all words in the document"""
    return re.findall(r"\w+", document[1])

def toPairs(word):
    """ Creates `(key, value)` pairs where the word is the key and 1 is the value """
    return (word, 1)

def sumCounts(a, b):
    """ Add up the values for each word, resulting in a count of occurences """
    return a + b

""" TODO: Add functions here to determine the most popular word
    Note that Map/flatMap style functions take in one argument while Reduce functions take in two
"""
def remove_articles_pair(word):
    ##because we want to find the most popular non-article word, we should ignore "a" "an" "the"
    lower_word = word[0].lower()
    if( lower_word == "a" or lower_word == "an" or lower_word == "the"):
        return False
    else:
        return True
def map_again(arg):
    ##after this mapping, we can make every elements have the same key, so that we can find the max element in the next step through the function reduceBykey because they already has the same key
    return ("same",(arg[1],arg[0]))
def find_the_max(a,b):
    if(a[0]>b[0]):##at this time a[0] and b[0] are arg[1], that is the times of the word appearance
        return a
    else:
        return b
def mostPopular(file_name, output="spark-wc-out-mostPopular"):
    sc = SparkContext("local[8]", "WordCount", conf=SparkConf().set("spark.hadoop.validateOutputSpecs", "false"))
    """ Reads in a sequence file FILE_NAME to be manipulated """
    file = sc.sequenceFile(file_name)

    counts = file.flatMap(splitDocument) \
                 .map(toPairs) \
                 .reduceByKey(sumCounts).sortByKey().filter(remove_articles_pair).map(map_again).reduceByKey(find_the_max)
                 # after sorting, we can remove the invalid elements and then 
                 # TODO: add appropriate extra transformations here

    """ Takes the dataset stored in counts and writes everything out to OUTPUT """
    counts.coalesce(1).saveAsTextFile(output)

""" Do not worry about this """
if __name__ == "__main__":
    argv = sys.argv
    if len(argv) == 2:
        mostPopular(argv[1])
    else:
        mostPopular(argv[1], argv[2])

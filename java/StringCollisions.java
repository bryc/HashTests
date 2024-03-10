/*
Testing large collections of unique strings for collisions in 32-bit hashes
---------

# Premise ----

We know that collisions are inevitable when enough hashes are produced.
If we assume that the best odds of a collision would be that of pure random chance,
we are able to measure any deviations from this assumption.

We can calculate the average number of expected collisions after n hash calculations using:

    mean = (n*n - n) / (Math.pow(2, 32) * 2)
    
For reasons not entirely clear, the expected mean is also the expected variance. We can use
this directly in calculating the standard deviation by simply square rooting the mean:

    sd = sqrt(mean)
    
This value then represents the standard deviation that models the expected normal distrubtion
for any particular mean.

With this we can then calculate z-scores based on observed number of collisions:

    score = (collisions - mean) / sd
    
This value then tells us how many standard deviations the result is from the expectation.

# The test ----

A text file is provided which contains a large number of unique strings, one per line.
Ideally, to test collisions in 32-bit hash functions, we want at least 1 million stirngs.
This is to ensure some number of collisions will be found that we can compare against.

So for example, the expected number of collisions after 1 million hashes is:

    116.415205411612987518310546875
    
The results we would expect for real randomness would be between 96 and 136 collisions.
Representing -1.8 and +1.8 standard deviations respectively.

It is not rare to see values as high as 2.7 or even 3.3 standard deviations, however the likelihood
decreases exponentially. I have found 4.5 to be a useful threshold for unambigious outliers.

They are such a rare result that if their probability is noticably higher than expected, we can 
quickly determine that the hash function exhibits a measurably higher collision rate.


# Shortcomings of this test ----

A single z-score has limited value unless a hash function is so biased that all possible z-scores
represent unambigious outliers.

I have realized that it is trivial to produce simple multiplicative hashes with collision
counts often closer to the expectation. And so there is a risk of false negatives if we do not
have enough data to be confident in our measurement.

This means that a single z-score is often not enough to confirm if the overall collision resistance
is as good as the expectation under all possible scenerios.

And so accumulating z-scores from multiple rounds is often necessary to weed out any outliers
that may not be identified with a single z-score. And in fact, a large number of z-scores
may be required for truly meaningful results.

The problem with calculating multiple z-scores is that it vastly increases the calculation time.
And I currently don't have a useful measure for accumulating z-scores, other than merely
determining the min and max.

*/
import java.io.File;
import java.nio.file.Files;
import java.io.IOException;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

public class HashTest {
    public static int Hash(char[] key, int seed) {
        int hash = seed;
        for (int i = 0; i < key.length; i++) {
            hash += key[i];
            hash += hash << 7;
        }
        return hash;
    }
    private static Map<Integer, String> hashes(Collection<String> values, int seed) {
        Map<Integer, String> result = new HashMap<>();
        values.forEach(value -> result.put(Hash(value.toCharArray(), seed), value));
        return result;
    }
    public static void main(String[] args) throws IOException {
        Collection<String> lines = Files.readAllLines(new File(args[0]).toPath());
        Map<Integer, String> hashes;
        double n = lines.size(), mean = (n*n - n) / (Math.pow(2, 32) * 2), sd = Math.sqrt(mean);
        double collisions = 0, score = 0;
        int count = 1;
        long start = System.nanoTime();
        for(int i = 0; i < count; i++) {
            hashes = hashes(lines, 1);
            collisions = n - hashes.size();
            score = (collisions - mean) / sd;
            
            String color = "\u001B[32m";
            if(Math.abs(score) >= 2) color = "\u001B[36m";
            if(Math.abs(score) >= 2.5) color = "\u001B[93m";
            if(Math.abs(score) >= 3) color = "\u001B[33m";
            if(Math.abs(score) >= 3.6) color = "\u001B[31m";
            if(Math.abs(score) >= 4.5) color = "\u001B[30m\u001B[41m";
            
            System.out.println(
                String.format("%5d, ", (int)collisions) +
                String.format("%5d, ", (int)mean) + color + 
                (score+"").substring(0, 12) + "\u001B[0m - "  + 
                args[0].substring(7) + " (" + (int)n + ")"
            );
        }   
        long finish = System.nanoTime(), elapsed = finish - start;
    }
}

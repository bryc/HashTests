import java.io.File;
import java.nio.file.Files;
import java.nio.charset.StandardCharsets;
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
        Collection<String> lines = Files.readAllLines(new File(args[0]).toPath(), StandardCharsets.ISO_8859_1);
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
            if(Math.abs(score) >= 2.0) color = "\u001B[36m";
            if(Math.abs(score) >= 2.5) color = "\u001B[93m";
            if(Math.abs(score) >= 3.0) color = "\u001B[33m";
            if(Math.abs(score) >= 3.6) color = "\u001B[31m";
            if(Math.abs(score) >= 4.5) color = "\u001B[30m\u001B[41m";
            
            System.out.println(
                String.format("%5.0f, ", mean) +
                String.format("%5d, ", (int)collisions) + color + 
                (score+"").substring(0, 12) + "\u001B[0m "  + 
                args[0].substring(7) + " (" + (int)n + ")"
            );
        }
        long finish = System.nanoTime(), elapsed = finish - start;
    }
}

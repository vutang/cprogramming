/******************************************************************************
 *  Compilation:  javac Rolls.java
 *  Execution:    java Rolls n
 *  Dependencies: Counter.java StdRandom.java StdOut.java
 *
 *  Simulate the rolling of n 6-sided dice.
 *
 *  % java Rolls 1000000
 *  167308 1s
 *  166540 2s
 *  166087 3s
 *  167051 4s
 *  166422 5s
 *  166592 6s
 *
 ******************************************************************************/

public class Rolls {
    public static void main(String[] args) {
        int n = Integer.parseInt(args[0]);
        int SIDES = 6;

        // initialize counters
        Counter[] rolls = new Counter[SIDES+1];
        for (int i = 1; i <= SIDES; i++) {
            rolls[i] = new Counter(i + "s");
        }

        // flip dice
        for (int j = 0; j < n; j++) {
            int result = StdRandom.uniform(1, SIDES+1);
            rolls[result].increment();
        }

        // print results
        for (int i = 1; i <= SIDES; i++) {
            StdOut.println(rolls[i]);
        }
    }
}


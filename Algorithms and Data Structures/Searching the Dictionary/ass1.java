/*
	Assignment 1
	Name: Thi Thuy Trang Nguyen
	Student login: tttn941
*/

import java.util.Scanner;
import java.io.IOException;
import java.io.FileInputStream;
import java.lang.Math;
import java.util.*;

public class ass1
{
	
	//implementing a 0-based array of words read from file
	private static String[] dictionary = new String[400000];
	private static int lastWord = 0;   //number of elements in the array
	private static int noOfPalindromes = 0; //no of palindromes
	
	public static String[] uniqueWordsRead = new String[1000]; //a 1-based array contains of unique words read from sample.txt
	private static int totalValidWords = 0;
	public static int totalUniqueWords = 0;
	
	private static int totalWordsInDict = 0; //number of unique words that appear in both files
	private static String[] wordsInDict = new String[1000]; //1-based array of unique words appear in both files
	
	private static String[][] anagrams = new String[1000][100]; //An array of 1-based arrays of anagrams 
	private static int[] noOfAnagrams = new int[1000]; //corresponding number of anagrams of each word
	private static int noOfWordsWithAnagrams = 0;
	private static String[] wordsWithAnagram = new String[1000]; //Words that have anagrams
	private static String wordWithMostAnag;
	private static String longestWordWithAnag; 
	private static int totalAnags = 0; //total number of anagrams found
	
	public static void main(String[] args)
	{
		
		try{
			
			long startTime = System.nanoTime(); //record the start time
			
			Scanner input = new Scanner(new FileInputStream("dictionary.txt"));
			
			while (input.hasNext())
			{
				//insert into the array from file
				String word = input.nextLine();
				dictionary[lastWord] = word;
				lastWord++; 
			}

			input.close();	//close file
			
			System.out.println("Number of words in dictionary: " + lastWord);
			/*
			//Step 1 - Linear search
			
			for (int i = 0; noOfPalindromes < 5; i++)
			{
				int len = dictionary[i].length();
				if (len > 1)
				{
					String reversedString = reverse(dictionary[i]);
					int index = linearSearch(reversedString, dictionary, lastWord);
					if(index != -1) //if the linearSearch finds a palindrome
					{
						System.out.println(dictionary[i] + " : " + dictionary[index]);
						noOfPalindromes++;
					}
				}
			}
			*/
			
			//Step 2 - Binary search
		
			int longestPalindromeLength = 2;
			int longestPalindromeIndex = 0;

			for (int i = 0; i < lastWord; i++)
			{
				String word = dictionary[i];
				int len = word.length();
				if (len > 1)
				{
					String reversedString = reverse(word);
					if (reversedString.compareTo(word) == 0) //if the word is symmetrical, no binary search needed
					{
						noOfPalindromes++;
						if (noOfPalindromes >= 1 && noOfPalindromes <= 5) //print first 5 pairs of palindromes
							System.out.println(word + " : " + word);
						if (longestPalindromeLength < len)
						{
							longestPalindromeLength = len;
							longestPalindromeIndex = i;
						}
					}else{
						//String reversedString = reverse(word);
						int index = binarySearch(0, lastWord-1, dictionary, reversedString);
						if(index != -1) //if the linearSearch finds a palindrome
						{
							noOfPalindromes++;
							if (noOfPalindromes >= 1 && noOfPalindromes <= 5) //print first 5 pairs of palindromes
								System.out.println(word + " : " + reversedString);
							if (longestPalindromeLength < len)
							{
								longestPalindromeLength = len;
								longestPalindromeIndex = i;
							}
						}
					}
				}
			}
			System.out.println("The longest palindrome found: " + dictionary[longestPalindromeIndex]);
			
			//Step 3 - Spell check
			//Preprocess sample.txt
						
			input = new Scanner(new FileInputStream("sample.txt"));
			AVLTree tree = new AVLTree(); 
			while (input.hasNext())
			{
				String inStr = input.next();
				String words[] = inStr.replaceAll("[^a-zA-Z ]", "").toLowerCase().split("\\s+");
				String word = words[0];
				totalValidWords++;
				
				tree.root = tree.insert(tree.root, word);
			}
			tree.inOrder(tree.root); //sort the unique words array
			System.out.println("Number of valid words in sample.txt: " + totalValidWords);
			System.out.println("Number of unique words in sample.txt: " + totalUniqueWords);
			
			//count number of unique words found in the dictionary and add to the array, which is sorted according to the unique words array
			for (int i = 1; i <= totalUniqueWords; i++)
			{
				if(binarySearch(0, lastWord-1, dictionary, uniqueWordsRead[i]) != -1)
				{
					totalWordsInDict++;
					wordsInDict[totalWordsInDict] = uniqueWordsRead[i];
				}
			}
			System.out.println("Number of unique words found in dictionary: " + totalWordsInDict);
			
			
			//Step 4 - Find anagrams
			int mostAnag = 0;
			int longestWord = 0;
			for (int i = 1; i <= totalWordsInDict; i++)
			{
				String s1 = wordsInDict[i];
				int len = s1.length();
				if (len == 2)
				{
					String reversed = reverse(s1);
					int result = binarySearch(0, lastWord, dictionary, reversed);
					if (result != -1)
					{
						noOfWordsWithAnagrams++; //counting the number of words with anagrams
						wordsWithAnagram[noOfWordsWithAnagrams] = s1;
						//insert the anagram
						noOfAnagrams[noOfWordsWithAnagrams]++; //increment the counter of no of anagrams of the word
						int no = noOfAnagrams[noOfWordsWithAnagrams];
						anagrams[noOfWordsWithAnagrams][no] = reversed;		//add the anagram to the heap		
					}
				}else if (len > 2) {
					for (int j = 0; j < lastWord; j++)
					{
						String s2 = dictionary[j];
						boolean isAnag = isAnagram(s1, s2);
						if(isAnag == true)
						{
							if (!s1.equals(wordsWithAnagram[noOfWordsWithAnagrams])) //if the word hasn't been added to the heap
							{
								noOfWordsWithAnagrams++; //counting the number of words with anagrams
								wordsWithAnagram[noOfWordsWithAnagrams] = s1;
	
							}
							//insert the anagram
							noOfAnagrams[noOfWordsWithAnagrams]++; //increment the counter of no of anagrams of the word
							int no = noOfAnagrams[noOfWordsWithAnagrams];
							anagrams[noOfWordsWithAnagrams][no] = s2;		//add the anagram to the heap								
						}
					}
					heapSort(anagrams[noOfWordsWithAnagrams], noOfAnagrams[noOfWordsWithAnagrams]); //sort the anagrams of each word
				}
			}
						
			
			//Find the word with the most anagrams and the longest word that has anagrams
			System.out.println();
			System.out.println("The first 10 anagram words and their anagrams:");
			for (int i = 1; i <= noOfWordsWithAnagrams; i++)
			{
				if (mostAnag < noOfAnagrams[i])
				{
					mostAnag = noOfAnagrams[i];
					wordWithMostAnag = wordsWithAnagram[i];
				}
				if (longestWord < wordsWithAnagram[i].length())
				{
					longestWord = wordsWithAnagram[i].length();
					longestWordWithAnag = wordsWithAnagram[i];
				}
				totalAnags += noOfAnagrams[i]; //counting total anagrams found
				
				//print first 10 anagrams			
				if (i>=1 && i <= 10)
				{
					System.out.print(wordsWithAnagram[i] +": ");
					for (int j = 1; j <= noOfAnagrams[i]; j++)
					{
						System.out.print(anagrams[i][j] + " ");
					}
					System.out.println();			
				}
				
			}
			System.out.println("The word with the most anagrams: " + wordWithMostAnag);
			System.out.println("The longest word with anagram(s): " + longestWordWithAnag);
			System.out.println("Total number of words with anagram(s): " + noOfWordsWithAnagrams);
			System.out.println("Total number of anagrams found: " + totalAnags);
			
			//calculating run time
			long endTime = System.nanoTime();
			double totalTime = (endTime - startTime) * Math.pow(10, -9);
			System.out.printf("Total run time (secs): %.2f\n", totalTime);
			
		}catch(IOException e){
			System.err.println("File fails to open. Terminating...");
			System.exit(1);
		}
		
	}
	
	//Step 1
	public static String reverse(String s) //reverse a string
	{
		char temp[] = s.toCharArray(); //convert the string to a char array
		int len = s.length();
		for(int i = 0; i <= (len-1)/2; i++) //swap the char, first and last...
		{
			if(temp[i] != temp[len-i-1])
			{
				char dummy = temp[i];
				temp[i] = temp[len-i-1];
				temp[len-i-1] = dummy;
			}
		}
		return new String (temp);
	}
	
	
	public static int linearSearch(String s, String array[], int length)
	{		
		for (int i = 0; i < length - 1; i++)
		{
			String word = array[i];
			if(s.equals(word))
			{
				return i; //return the index of the string found
			}
		}
		return -1; //return -1 if the string is not found
	}
	
	//step 2
	
	public static int binarySearch(int start, int end, String array[], String newStr)
	{
		int mid = (start + end)/2;
		if (start <= end)
		{
			String word = array[mid];
			if(newStr.equals(word))//if the inserted word already exists
			{	
				return mid;
			}else if (newStr.compareTo(word) > 0){
				return binarySearch(mid+1, end, array, newStr);
			}else{
				return binarySearch(start, mid-1, array, newStr);
			}	
		}
		
		return -1;
	}
	
	//Step 4 - Anagrams
	
	public static void swap(int i, int j, String[] array) //when swaping words in the heap, its occurrence in the corespondant array must be swapped too
	{
		String temp = array[i];
		array[i] = array[j];
		array[j] = temp;		
	}
	
	public static void siftdown(int i, int leaf, String heap[]) //move element to its correct position
	{
		int child = i*2;
		if (child <= leaf)
		{	
			if (child + 1 <= leaf)
			{
				if (heap[child].compareTo(heap[child+1]) < 0)
				{
					child++;
				}	
			}
			if (heap[i].compareTo(heap[child]) < 0)
			{
				swap(i, child, heap);
				siftdown(child, leaf, heap);
			}
		}
	}
	public static void makeheap(String[] array, int noOfElements) //convert an array into a heap
	{
		for(int i = (noOfElements - 1)/2; i >= 1; i--)
		{
			siftdown(i, noOfElements -1, array);
		}
	}
	
	public static void heapSort(String[] array, int noOfElements)
	{
		makeheap(array, noOfElements);
		for (int i = noOfElements - 1; i >= 2; i--)
		{
			swap(1, i, array);
			siftdown(1, i-1, array);
		}
	}
	
	public static boolean isAnagram(String s1, String s2)
	{
		int product1 = 1;
		int product2 = 1;
		if (s1.length() != s2.length() || s1.equals(s2))
		{
			return false;
		}
		for(char c: s1.toCharArray())
		{
			product1 = product1 * charToInt(c); //calc product of all char
		}
		for(char c: s2.toCharArray())
		{
			product2 = product2 * charToInt(c); //calc product of all char
		}
		
		if (product1 != product2)
		{
			return false;
		}
		
		return true;
	}
	public static int charToInt(char c) //convert a char to a prime number
	{
		switch (c)
		{
			case 'a':
				return 2;
			case 'b':
				return 3;
			case 'c':
				return 5;
			case 'd':
				return 7;
			case 'e':
				return 11;
			case 'f':
				return 13;
			case 'g':
				return 17;
			case 'h':
				return 19;
			case 'i':
				return 23;
			case 'j':
				return 31;
			case 'k':
				return 37;
			case 'l':
				return 41;
			case 'm':
				return 43;
			case 'n':
				return 47;
			case 'o':
				return 53;
			case 'p':
				return 59;
			case 'q':
				return 61;
			case 'r':
				return 71;
			case 's':
				return 73;
			case 't':
				return 79;
			case 'u':
				return 83;
			case 'v':
				return 89;
			case 'w':
				return 97;
			case 'x':
				return 101;
			case 'y':
				return 103;
			case 'z':
				return 107;
			default:
				System.out.println("The case is not defined");
				return -1;
		}
	}
}
	
//Step 3 - Spell check
class Node { 
    public int height; 
	public String key;
    public Node left, right; 
  
    public Node(String d) //constructor
	{ 
        key = d; 
        height = 1; 
    } 
} 
  
class AVLTree { 
  
    public Node root; 

	public AVLTree() // constructor
	{
		
	}

    public int height(Node N) //get the height of the tree 
	{ 
        if (N == null) 
            return 0; 
  
        return N.height; 
    } 
  
    public int max(int a, int b) //get maximum of two integers 
	{ 
        return (a > b) ? a : b; 
    } 
  
    public Node rightRotate(Node y) //right rotate subtree with root y
	{ 
        Node x = y.left; 
        Node T2 = x.right; 
  
        // Perform rotation 
        x.right = y; 
        y.left = T2; 
  
        // Update heights 
        y.height = max(height(y.left), height(y.right)) + 1; 
        x.height = max(height(x.left), height(x.right)) + 1; 
  
        // Return new root 
        return x; 
    } 
  
    // Left rotate subtree rooted with x 
    public Node leftRotate(Node x) { 
        Node y = x.right; 
        Node T2 = y.left; 
  
        // Perform rotation 
        y.left = x; 
        x.right = T2; 
  
        //  Update heights 
        x.height = max(height(x.left), height(x.right)) + 1; 
        y.height = max(height(y.left), height(y.right)) + 1; 
  
        // Return new root 
        return y; 
    } 
  
    // Get Balance factor of node N 
    public int getBalance(Node N) { 
        if (N == null) 
            return 0; 
  
        return height(N.left) - height(N.right); 
    } 
  
    public Node insert(Node node, String key) { 
  
        //Perform the normal BST insertion 
        if (node == null) 
            return (new Node(key)); 
  
        if (key.compareTo(node.key) < 0) 
            node.left = insert(node.left, key); 
        else if (key.compareTo(node.key) > 0) 
            node.right = insert(node.right, key); 
        else // Duplicate values not allowed 
            return node; 
  
        //Update height of this ancestor node
        node.height = 1 + max(height(node.left), 
                              height(node.right)); 
  
        int balance = getBalance(node); 
  
        // If this node becomes unbalanced, then there are 4 cases 
		//case 1 
        if (balance > 1 && key.compareTo(node.left.key) < 0)
            return rightRotate(node); //return the new root
  
		// case 2
        if (balance > 1 && key.compareTo(node.left.key) > 0) { 
		//double right rotate
            node.left = leftRotate(node.left); 
            return rightRotate(node); 
        } 
        //Case 3
        if (balance < -1 && key.compareTo(node.right.key) > 0)
            return leftRotate(node); 
  
        //Case 4 
        if (balance < -1 && key.compareTo(node.right.key) < 0) { 
		//double left rotate
            node.right = rightRotate(node.right); 
            return leftRotate(node); 
        } 
  
        /* return the (unchanged) node pointer */
        return node; 
    } 
  
	//in-order traversal of the tree
	//this sorts the tree in alphabetical order
	//it also adds the unique words read to a sorted array
    public void inOrder(Node node) { 
        if (node.left != null) { 
			inOrder(node.left); //visit left node
		}
		//add to the unique word array
		ass1.totalUniqueWords++;
		ass1.uniqueWordsRead[ass1.totalUniqueWords] = node.key; 
		
        if(node.right != null){ 
            inOrder(node.right); //visit right node
        } 
    } 
 
}

//Step 5 - Specifications
	/*
	**	RUN TIME OF PROGRAM: 29.49 secs and THE MACHINE IT WAS ON: banshee.cs.uow.edu.au	
	**	STEP 2 SPEED UP: 
		It takes 9.35 secs to find only 5 palindromes so to find all palindromes using linear search, this may take about 692,093 secs to look at all 370103 words in the dictionary.
		A binary search is about log(n)/n times faster than a linear search, where n is the number of words it has to look at. So to find all palindromes using a binary search, it may takes 692,093 * (log(370,103) / 370,103) = 10.41 secs.
		To speed up, for words that are symmetrical, their palindromes will be themselves. This saves time as opposed to using binary search for these words.
	**	TO SPELL CHECK: 2 arrays are used to store the unique words read from sample.txt and the unique words that also appear in the dictionary.
		To find unique words read from sample.txt, we need to check if a word has been added to the array before. We can use a linear search to see if the word has been read before but this takes too much time for a large number of words in the file.
		To speed up, I implement an AVL tree, do in-order traversal to sort the unique words alphabetically and store them in an array.
		To find the unique words that are also in the dictionary, I do a binary search because the dictionary has already been sorted.
	**	TO FIND PALINDROMES: a 2-dimendional array is used to store pairs of {palindrome, emordnilap}
		For each word in the dictionary, firstly, we reverse the order of the characters in that word using a loop. Then, using either a linear search or a binary search, we search the reversed words in the dictionary and add the {palindrome, emordnilap} pairs found to the multidimensional array to print them out. 
		(A binary search is more efficient in this case because the dictionary is already sorted alphabetically and each time we can eliminate half of the dictionary to find the emordnilaps.)
	**	TO FIND ANAGRAMS: A 1-based array containing words that have anagrams is implemented. Also, a 2-dimensional array contains arrays of anagrams of the words in the aforementioned array. The index of each word in the array corresponds to the index of an array of its anagrams inside the 2-dimensional array.
		For instance, a word at position wordsWithAnagram[1] has an array of anagrams at position anagrams[1]; and anagrams[1][3] can be 1 of the anagrams of this word.
		An array of the number of anagrams each word has is used. Similar to the 2-dimensional array of anagrams, the index of each element of this array corresponds to the index of each word in the array of anagram words.
		For each unique word that appears in both the sample.txt and dictionary.txt, we count the occurrence of its characters.
		SPEED ENHANCEMENT: Instead of using the 2 algorithms presented in the lecture to find anagrams, for each char in a word, I converted it to a prime number and calculate the product of all characters in both strings. If the 2 products are equal, they are anagrams. This saves the time of having to iterate throught a 26-element character arrays each time a word is checked.
		For a 2-character word, I reversed the word and use binary search to find it in the dictionary, which also saves time as opposed to using a linear search for these words.
		Using a linear search, for each unique word, find its anagrams in the dictionary and add them to the 2-dimensional array in the correct position. Also, increment the number of anagrams of each word and count the total number of words that have anagram(s).
		Using heap sort, which calls siftdown and makeheap - a function that converts an array to a max heap, we can sort the array of words with anagrams as well as the arrays of anagrams of each word efficiently.
		Lastly, using a loop, we can find the longest word with anagram(s), the word with the most anagrams and the total anagrams found.
	*/
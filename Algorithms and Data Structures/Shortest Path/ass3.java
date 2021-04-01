/*
	Assignment 3 - Dijkstra's Algorithm
	Name: Thi Thuy Trang Nguyen
	Student login: tttn941
*/

import java.util.Scanner;
import java.io.IOException;
import java.io.FileInputStream;
import java.util.Arrays;

class Vertex
{
	public char label;
	public int x, y;
	
	public Vertex(char l, int xO, int yO)
	{
		label = l;
		x = xO;
		y = yO;
	}
}

class Matrix
{
	private final int noOfVertices;
	private int noOfEdges;
	private double matrix[][];
	private Vertex vertices[];
	
	//for dijkstra alg
	private int noVisited = 0;
	private int shortestPath[];
	private int verticesOnShortestPath = 0;
	
	
	public Matrix(int v, int e)
	{
		noOfVertices = v;
		noOfEdges = e;
		matrix = new double[noOfVertices+1][noOfVertices+1];
		vertices = new Vertex[noOfVertices+1];
	}
	public static int labelToInt(char c){ //convert char to int with a=1...z=26
		return (c - 'a' + 1);
	}		
	public void addEdgeToList(char src, char dest, double cost) //directed
	{
		int srcInt = labelToInt(src);
		int destInt = labelToInt(dest);
		matrix[srcInt][destInt] = cost;
	}
	public void addVertexToList(char label, int x, int y) //directed
	{
		int pos = labelToInt(label);
		vertices[pos] = new Vertex(label,x,y);
		//initialise the matrix with infinities
		for(int i= 1; i <= noOfVertices; i++)
		{
			if(i == pos){
				matrix[pos][pos] = 0;
			}else{
				matrix[pos][i] = Double.POSITIVE_INFINITY;
			}
		}
	}
		
	//Step 1 - print first 5 vertecies
	public void printFive(){			
		for(int i = 1; i <= 5; i++)
		{
			System.out.print(vertices[i].label + ":\t");		
			for(int j = 1; j <= noOfVertices; j++)
			{
				if(matrix[i][j] != Double.POSITIVE_INFINITY && i!=j)
				{
					System.out.print(vertices[j].label + "(" + matrix[i][j] + ")\t");	
				}					
			}
			System.out.println();
		}
	}
	
	//Step 2- Dijkstra
	public double[] Dijkstra(char src, char dest, int P[])
	{
		noVisited = 0;
		int s = labelToInt(src); //source and destination vertices from file
		int d = labelToInt(dest);
		double D[] = new double[noOfVertices+1]; //smallest weights
		boolean S[] = new boolean[noOfVertices+1]; //selected
		S[1] = true;
		for(int i = 2; i <= noOfVertices; i++){
			D[i] = matrix[s][i]; //initialise from source
			P[i] = s;
		}
		int v = minNotSelected(S, D);
		
		while(v != 0 && !S[d]){
			for(int u = 1; u <= noOfVertices; u++){
				if(D[u] > D[v] + matrix[v][u] && S[u] == false){
					D[u] = D[v] + matrix[v][u];
					P[u] = v;
				}
			}
			v = minNotSelected(S, D);			
		}
		
		return D;
	}
	
	public void printPath(char s, char d, int P[])
	{
		int src = labelToInt(s); //source and destination vertices
		int dest = labelToInt(d);
		int count = 1;
		int current = dest;
		
		shortestPath = new int[noOfVertices+1];
		shortestPath[count] = current;
		while(current != src && count <= noOfVertices){
			count++;
			current = P[current];
			shortestPath[count] = current;
		}
		System.out.print("Path: ");
		for(int i=count; i >= 1; i--){
			System.out.print(vertices[shortestPath[i]].label + " ");
		}
		verticesOnShortestPath = count;				
	}
	public void getPathDistance(double distance)
	{
		System.out.println("\nPath distance: " + distance);
	}
	public void getNoVerticesVisited()
	{
		System.out.println("Number of vertices visited: " + noVisited);
	}
	public int minNotSelected(boolean S[], double D[])
	{
		int index = 0;
		double min = Double.POSITIVE_INFINITY;
		for(int i = 1; i <= noOfVertices; i++){
			if(S[i] == false && D[i] < min){
				min = D[i];
				index = i;
			}
		}
		noVisited++;
		S[index] = true;
		return index;
	}
	
	//Step 3
	public double[] secondShortestPath(char src, char dest, int P[])
	{
		double otherPaths[][] = new double[noOfVertices+1][noOfVertices+1];
		int index = 0, visited = 0;
		int tempPath[] = P;
		double shortest = Double.POSITIVE_INFINITY;
		for(int i=1; i < verticesOnShortestPath; i++){
			double temp = matrix[shortestPath[i+1]][shortestPath[i]] ;
			matrix[shortestPath[i+1]][shortestPath[i]] = Double.POSITIVE_INFINITY;
			otherPaths[i] = Dijkstra(src, dest, P);
			matrix[shortestPath[i+1]][shortestPath[i]] = temp;
			if(otherPaths[i][labelToInt(dest)] < shortest){
				shortest = otherPaths[i][labelToInt(dest)];
				index = i;
				visited = noVisited;
				tempPath = Arrays.copyOf(P, P.length);
			}
		}
		P = Arrays.copyOf(tempPath, tempPath.length);
		noVisited = visited;
		printPath(src,dest,P);
		
		return otherPaths[index];
	}
	
	//Step 4
	public double calcDistance(Vertex v, Vertex dest)
	{
		return Math.sqrt(Math.pow(v.x - dest.x,2) + Math.pow(v.y - dest.y,2));
	}
	public int minNotSelectedAStar(boolean S[], double D[], int d)
	{
		int index = 0;
		double min = Double.POSITIVE_INFINITY;
		for(int i = 1; i <= noOfVertices; i++){
			if(S[i] == false && D[i] + calcDistance(vertices[i], vertices[d]) < min){
				min = D[i] + calcDistance(vertices[i], vertices[d]);
				index = i;
			}
		}
		S[index] = true;
		noVisited++;
		return index;
	}
	public double[] AStar(char src, char dest, int P[])
	{
		noVisited = 0;
		int s = labelToInt(src); //source and destination vertices from file
		int d = labelToInt(dest);
		double D[] = new double[noOfVertices+1]; //smallest weights
		boolean S[] = new boolean[noOfVertices+1]; //selected
		S[1] = true;
		for(int i = 2; i <= noOfVertices; i++){
			D[i] = matrix[s][i]; //initialise from source
			P[i] = s;
		}
		int v = minNotSelectedAStar(S, D, d);
		
		while(v != 0 && !S[d]){
			for(int u = 1; u <= noOfVertices; u++){
				if(D[u] > D[v] + matrix[v][u] && S[u] == false){
					D[u] = D[v] + matrix[v][u];
					P[u] = v;
				}
			}
			v = minNotSelectedAStar(S, D, d);			
		}
		return D;
	}
	public double[] secondShortestPathAStar(char src, char dest, int P[])
	{
		double otherPaths[][] = new double[noOfVertices+1][noOfVertices+1];
		int index = 0, visited = 0;
		int tempPath[] = P;
		double shortest = Double.POSITIVE_INFINITY;
		for(int i=1; i < verticesOnShortestPath; i++){
			double temp = matrix[shortestPath[i+1]][shortestPath[i]] ;
			matrix[shortestPath[i+1]][shortestPath[i]] = Double.POSITIVE_INFINITY;
			otherPaths[i] = AStar(src, dest, P);
			matrix[shortestPath[i+1]][shortestPath[i]] = temp;
			if(otherPaths[i][labelToInt(dest)] < shortest){
				shortest = otherPaths[i][labelToInt(dest)];
				index = i;
				visited = noVisited;
				tempPath = Arrays.copyOf(P, P.length);
			}
		}
		P = Arrays.copyOf(tempPath, tempPath.length);
		noVisited = visited;
		printPath(src,dest,P);
		
		return otherPaths[index];
	}
}

public class ass3
{
	public static void main(String[] args) throws CloneNotSupportedException
	{
		
		Scanner sc = new Scanner(System.in);
		
		try{
			Scanner input = new Scanner(new FileInputStream("ass3.txt"));
			final int noOfVertices = input.nextInt();
			final int noOfEdges = input.nextInt();
			Matrix matrix = new Matrix(noOfVertices, noOfEdges);
			for(int i = 1; i <= noOfVertices; i++)
			{
				char src = input.next().charAt(0);
				int x = input.nextInt();
				int y = input.nextInt();
				matrix.addVertexToList(src, x, y);
			}
			for(int i = 1; i <= noOfEdges; i++)
			{
				char src = input.next().charAt(0);
				char dest = input.next().charAt(0);
				double cost = input.nextInt();
				matrix.addEdgeToList(src, dest, cost);
			}
			char src = input.next().charAt(0);
			char dest = input.next().charAt(0);
			input.close();	//close file
			
			//matrix.printFive(); //step 1
			int P[] = new int[noOfVertices+1];
			System.out.println("Start and end vertex: " + src + " " + dest);
			
			//Step 2
			System.out.println("\nShortest path using Dijkstra alg:");
			double D1[] = matrix.Dijkstra(src,dest,P);
			matrix.printPath(src, dest, P);
			matrix.getPathDistance(D1[matrix.labelToInt(dest)]);
			matrix.getNoVerticesVisited();
			
			//Step 3
			System.out.println("\nSecond shortest path using Dijkstra alg:");
			double D2[] = matrix.secondShortestPath(src,dest,P);
			matrix.getPathDistance(D2[matrix.labelToInt(dest)]);
			matrix.getNoVerticesVisited();
			
			//Step 4
			System.out.println("\nShortest path using A* alg:");
			double D3[] = matrix.AStar(src,dest,P);
			matrix.printPath(src, dest, P);
			matrix.getPathDistance(D3[matrix.labelToInt(dest)]);
			matrix.getNoVerticesVisited();
			
			System.out.println("\nSecond shortest path using A*	alg:");
			double D4[] = matrix.secondShortestPathAStar(src,dest,P);
			matrix.getPathDistance(D4[matrix.labelToInt(dest)]);
			matrix.getNoVerticesVisited();
			
		}catch(IOException e){
			System.err.println("File fails to open. Terminating...");
			System.exit(1);
		}
	}
}

/* Step 5
-To represent the graph, I used an adjacency matrix implemented using a direct-access 2 dimentional array.
-Instead of using the ASCII code of the vertex's lebel as an index to directly access the matrix, which is huge, 
 I used a function that change the value of the label to a smaller integer from 1 to 26, with a=1 and z=26.
-I used a boolean array to record the visited nodes. If node i is visited, then S[i] = true
-A double array was used to record the distance from the start vertex to every other vertices in the graph.
 If there's no path from the start vertex to vertex i, then D[i] = infinity

I used the proposed solution to the second shortest path problem:
	find the shortest path and store it in an array
	minCost = infinity
	for each edge ei on the shortest path
		store the cost of edge ei in the graph
		remove the edge by set the cost of edge ei in the graph to infinity
		find the shortest path from the start to the goal without edge ei using Dijkstra's algorithm
		if(minCost > the cost of the shortest path without edge ei)
			minCost = currentCost
			record the number of visited nodes and the path
		end if
	end for
	
The algorithm goes through all but one edge. If the second shortest path must be longer than the shortest patha and there's another path/edge with the same cost, the algorithm will fail.
We can repeat the same algorithm and remove each edge in the path that we have just found together with each edge of the shortest path,
 compare the cost of the new second shortest path to the paths found before and return the one with the lowest cost.
 We will need to keep track of the paths we visited, which can get very huge. The worst case is when all paths have the same cost.
 
Since the algorithm uses Dijkstra's algorithm, it will fail if the graph has cycles. But we can replace the Dijktra's algorithm with Belman-Ford algorithm to find the second shortest path.

The algorithm will still work with an undirected graph. Since an undirected graph is simply just a normal graph with bidirectional connections and the algorithm relies on Dijkstra's performance,
it still can work with an undirected graph.
*/

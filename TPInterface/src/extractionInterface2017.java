import java.io.*;
import java.lang.reflect.*;
//import java.nio.file.Files;
import java.util.*;
import java.util.Map.Entry;
import java.lang.reflect.Modifier;

public class extractionInterface2017 {

		
		private ArrayList<String> CharacteristicList;
		private ArrayList<String> EntityList;
		private boolean[][] table;
		
		public void createTable(int i) throws SecurityException, ClassNotFoundException{
			// Characteristic list and entity list are assigned
			if (CharacteristicList==null || EntityList==null) return;
			
			table = new boolean[EntityList.size()][CharacteristicList.size()];
			
			for (String e:EntityList)
				for (String c:CharacteristicList){
					switch(i){
						case 0:
							if (methodNames(e).contains(c))
								table[EntityList.indexOf(e)][CharacteristicList.indexOf(c)] = true;
							break;
						case 1:
							if (finalStaticFieldNamesTypes(e).contains(c))
								table[EntityList.indexOf(e)][CharacteristicList.indexOf(c)] = true;
							break;
						case 2:
							if (signatures(e).contains(c)) {
								table[EntityList.indexOf(e)][CharacteristicList.indexOf(c)] = true;
							}
							break;
						case 3:
							if (implementNames(e).contains(c)){
								table[EntityList.indexOf(e)][CharacteristicList.indexOf(c)] = true;
							}
					}
				}
		}
		
		public void afficheTable(){
			if (table == null) return;
			for (int k=0; k< table[0].length; k++){
				System.out.print(CharacteristicList.get(k)+"|");
			}
			System.out.println();
			for (int i=0; i< table.length; i++){
				System.out.print("|"+EntityList.get(i)+"|");
				for (int j=0; j< table[0].length; j++){
					if (table[i][j])
						System.out.print("x");
					System.out.print("|");
				}
				System.out.println();
			}
		}
		
		public void ecrireTable(String nomFichier) throws IOException{
			if (table == null) return;
			BufferedReader fc = new BufferedReader
			        (new InputStreamReader (System.in));
			BufferedWriter ff = new BufferedWriter
			              (new FileWriter (nomFichier));

			ff.write("FormalContext Collections"+"\n"+"| |");
			for (int k=0; k< table[0].length; k++){
				ff.write(CharacteristicList.get(k)+"|");
			}
			ff.newLine();;
			for (int i=0; i< table.length; i++){
				ff.write("|"+EntityList.get(i)+"|");
				for (int j=0; j< table[0].length; j++){
					if (table[i][j])
						ff.write("x");
					ff.write("|");
				}
				ff.newLine();;
			}
			ff.close(); 
		}
		
		public ArrayList<String> getCharacteristicList() {
			return CharacteristicList;
		}

		public void setCharacteristicList(ArrayList<String> characteristicList) {
			CharacteristicList = characteristicList;
		}

		public ArrayList<String> getEntityList() {
			return EntityList;
		}

		public void setEntityList(ArrayList<String> entityList) {
			EntityList = entityList;
		}

		public boolean[][] getTable() {
			return table;
		}

		public void setTable(boolean[][] table) {
			this.table = table;
		}
		
		public static List<Field> getAllFields(List<Field> fields, String className) throws ClassNotFoundException {
		    fields.addAll(Arrays.asList(Class.forName(className).getDeclaredFields()));

		    if (Class.forName(className).getSuperclass() != null) {
		        fields = getAllFields(fields, Class.forName(className).getSuperclass().getName());
		    }

		    return fields;
		}
		
		public static boolean isCollectionOrMap(String className) throws ClassNotFoundException {
			
			if(Class.forName(className).isAssignableFrom(Collection.class) || Class.forName(className).isAssignableFrom(Map.class)){			
				return true;
			}
			
			if(Class.forName(className).getInterfaces().length != 0){
				
				 if(isCollectionOrMap(Class.forName(className).getInterfaces()[0].getName())){
					 return true;
				 }
			}
			
			return false;
		}

		public static ArrayList<String> finalStaticFieldNamesTypes(String className) throws SecurityException, ClassNotFoundException
		{	
			ArrayList<String> liste = new ArrayList<>();
			//Object objectInstance = new Object();
				
			for (Field a : getAllFields(new LinkedList<Field>(), className))
			if (Modifier.isFinal(a.getModifiers()) && Modifier.isStatic(a.getModifiers())){		
				/*
				if(!a.isAccessible()){	
					a.setAccessible(true);
				}
				s+="\t" + a.getName()+" "+a.getType().getName()+" = "+a.get(objectInstance)+"\n";
				*/
				liste.add(a.getName()+" "+a.getType().getName());
			}
			
			return liste;
		}
		
		public static void createFile(String filename, String s) throws IOException{
			PrintWriter writer = new PrintWriter(filename, "UTF-8");
			writer.println(s);
			writer.close();
		}
		
		public static void createFileFromList(String filename, ArrayList<String> l) throws FileNotFoundException, UnsupportedEncodingException{
			PrintWriter writer = new PrintWriter(filename, "UTF-8");
			for(String s : l){
				writer.println(s);
			}
			
			writer.close();
		}
		
		public static void createFileAppend(String filename, String s) throws IOException{
			PrintWriter writer = new PrintWriter(new BufferedWriter(new FileWriter(filename, true)));
			
			writer.println(s);
			writer.close();
		}
		
		public static ArrayList<String> methodNames(String className)throws SecurityException, ClassNotFoundException
		{
			ArrayList<String> liste = new ArrayList<>();;
			for (Method m : Class.forName(className).getMethods())
				liste.add(m.getName());
			return liste;
		}
		
		public static ArrayList<String> implementNames(String className)throws SecurityException, ClassNotFoundException
		{
			ArrayList<String> liste = new ArrayList<>();
			
			Class<?> current = Class.forName(className);
			
			while(current != null){
				for(Class<?> i : current.getInterfaces()){
					//if(!isCollectionOrMap(i.getName())){
					String[] splt = i.getName().split("\\.");
					String name ="";
					if(splt.length != 0) {
						name = splt[splt.length - 1];
					}
					
					if(!liste.contains(name)){
						
						liste.add(name);
					}
					
				}
			current = current.getSuperclass();
				
			}
			
			return liste;
		}
		
		public static ArrayList<String> implementNamesSet(ArrayList<String> classNameList)throws SecurityException, ClassNotFoundException
		{
			ArrayList<String> liste = new ArrayList<>();
			
			for(String c : classNameList){	
				Class<?> current = Class.forName(c);
				
				while(current != null){
					for(Class<?> i : current.getInterfaces()){
						//if(!isCollectionOrMap(i.getName())){
							String[] splt = i.getName().split(".");
							String name ="";
							if(splt.length != 0) {
								name = splt[splt.length - 1];
							}
							if(!liste.contains(name)){
								
								liste.add(name);
							}
						//}
					}
					current = current.getSuperclass();
				}
			}
			
			return liste;
		}
		
		public static ArrayList<String> attributeNamesSet(ArrayList<String> classNameList)throws SecurityException, ClassNotFoundException
		{
			ArrayList<String> liste = new ArrayList<>();
			
			for(String c : classNameList){		
				for (Field f : getAllFields(new LinkedList<Field>(), c)){
					if (Modifier.isFinal(f.getModifiers()) && Modifier.isStatic(f.getModifiers())){
						if (!liste.contains(f.getName()+" "+f.getType().getName())){
							liste.add(f.getName()+" "+f.getType().getName());
						}
					}
				}
			}
			
			return liste;
		}
		
		public static ArrayList<String> methodNameSet(ArrayList<String> classNameList)throws SecurityException, ClassNotFoundException
		{
			ArrayList<String> liste = new ArrayList<>();
			for (String c: classNameList)
				for (Method m : Class.forName(c).getMethods())
					if (! liste.contains(m.getName())) liste.add(m.getName());
			return liste;
		}	
		
		// extrait toutes les signatures de toutes les classes de la liste passée en parametre
		public static ArrayList<String> signatureSet(ArrayList<String> classNameList)throws SecurityException, ClassNotFoundException
		{
			ArrayList<String> liste = new ArrayList<>();
			for (String c: classNameList)
				for (String signature : signatures(c))
					if (! liste.contains(signature)) liste.add(signature);
			liste.removeAll(signatures("java.lang.Object"));
			return liste;
		}	
		
		
		public static ArrayList<String> signatures(String className)throws SecurityException, ClassNotFoundException
		{
			ArrayList<String> liste = new ArrayList<>();
				

			for (Method m : Class.forName(className).getMethods())
			{
				String result ="";
				
				if((m.getModifiers() & Modifier.STATIC) !=0) {
					result+= "static ";
				}
			
				if(m.isDefault()) {
					result+= "default ";
				}
				result += m.getReturnType() + " ";
				result += m.getName() + "(";
				for (Class paramClass : m.getParameterTypes())
					result += paramClass.getName() + ", ";
				if(m.getParameterTypes().length != 0)
					result = result.substring(0, result.length()-2);
				result += ") ";
				/*
				for (Class exceptionType : m.getExceptionTypes())
					result += exceptionType.getName() + " ";
				if(m.getExceptionTypes().length != 0)
					result = result.substring(0, result.length()-1);
					*/
					
				liste.add(result);
				
			}
			return liste;
		}
		
		
		public static String methodNamesParamTypes(String className)throws SecurityException, ClassNotFoundException
		{
			String s="";
			System.out.println(Class.forName(className).getMethods().length);
			for (Method m : Class.forName(className).getMethods())
				{
					s+=m.getName()+"(";//+""+a.getType().getName()+"\n";
					for (Class paramType : m.getParameterTypes())
						s+=paramType.getName()+",";
					s+=")\n";
				}
			return s;
		}	
		
		public static String interfaceListNames(String[] classNameList)throws SecurityException, ClassNotFoundException, FileNotFoundException, UnsupportedEncodingException
		{			
			String s="";
			for (String c : classNameList)
				if (Class.forName(c).isInterface()){
					s+=c+"\n";
				}
				
			return s;
		}
		
		private static ArrayList<String> interfaceList(ArrayList<String> arrayList) throws ClassNotFoundException {
			ArrayList<String> liste = new ArrayList<>();
			for (String c : arrayList)
				if (Class.forName(c).isInterface())
					{liste.add(c);}
			return liste;
		}
		
		public static String abstractClassListNames(String[] classNameList)throws SecurityException, ClassNotFoundException, FileNotFoundException, UnsupportedEncodingException
		{	
			String s="";
			for (String c : classNameList)
				if (! Class.forName(c).isInterface() && Modifier.isAbstract(Class.forName(c).getModifiers())){
					s+=c+"\n";
				}
			
			return s;
		}
		
		public static ArrayList<String> abstractClassList(String[] classNameList)throws SecurityException, ClassNotFoundException, FileNotFoundException, UnsupportedEncodingException
		{	
			ArrayList<String> liste = new ArrayList<>();
			for (String c : classNameList)
				if (! Class.forName(c).isInterface() && Modifier.isAbstract(Class.forName(c).getModifiers()))
				{liste.add(c);}
			return liste;
		}
		
		/*private static String concreteClassListNames(String[] classNameList) throws ClassNotFoundException, UnsupportedEncodingException {
			
			String s=""; int nb =0;
			for (String c : classNameList)
				if (! Modifier.isAbstract(Class.forName(c).getModifiers())){
					s+=c+"\n";nb++;
				}
			
			System.out.println(nb+" classes concrètes");
			return s;
		}*/
		
		private static ArrayList<String> concreteClassList(String[] classNameList) throws ClassNotFoundException, IOException {
			
			ArrayList<String> liste = new ArrayList<>();
			for (String c : classNameList)
				if (! Modifier.isAbstract(Class.forName(c).getModifiers())){
					liste.add(c);
				}
			
			return liste;
		}
		
		private static ArrayList<String> concreteAbstractClassList(String[] classNameList) throws ClassNotFoundException, IOException {
			
			ArrayList<String> liste = new ArrayList<>();
			for (String c : classNameList)
				if (!Class.forName(c).isInterface()){
					liste.add(c);
				}
			
			return liste;
		}

		public static void ajoutPrefixe(String prefixe, String[] noms){
			for (int i=0; i < noms.length; i++)
				if (noms[i].equals("Object"))
					noms[i]= "java.lang."+noms[i];
				else
				noms[i]= prefixe+noms[i];			
		}
		
		/* TP1 */
		/* Partie 2 */
		// extrait tout les attributs de toutes les classes
		public static ArrayList<String> attributSet(ArrayList<String> classNameList, String modificateur)throws SecurityException, ClassNotFoundException
		{
			ArrayList<String> liste = new ArrayList<>();
			for (String c: classNameList)
				for (String atr : attributs(c, modificateur))
					if (! liste.contains(atr))
						liste.add(atr);
			//liste.removeAll(signatures("java.lang.Object"));
			return liste;
		}	
		// extrait les attributs d'une classe donnée avec des modificateurs donnés
		public static ArrayList<String> attributs(String className, String modificateur)throws SecurityException, ClassNotFoundException
		{
			ArrayList<String> liste = new ArrayList<>();
			
			Field[] fields = Class.forName(className).getDeclaredFields();
			String result ="";
			
			for (Field f : fields) {				
				if(Modifier.toString(f.getModifiers()).equals(modificateur) ) {
					result += Modifier.toString(f.getModifiers())+ " "+f.getName() + " " + f.getType() ;
					liste.add(result);				
				}
					
			}
			return liste;
		}
		
		//récupère les méthodes "static" de toutes les classes
		public static ArrayList<String> staticMethodes(ArrayList<String> classNameList)throws SecurityException, ClassNotFoundException
		{
			ArrayList<String> liste = new ArrayList<>();
			for (String c: classNameList)
				for (Method m : Class.forName(c).getMethods())
					if (! liste.contains(m.getName()) && (m.getModifiers() & Modifier.STATIC) !=0) 
						liste.add(m.getName());
			return liste;
		}	
		
		//récupère les méthodes par défaut de toutes les classes
		public static ArrayList<String> defaultMethodes(ArrayList<String> classNameList)throws SecurityException, ClassNotFoundException
		{
			ArrayList<String> liste = new ArrayList<>();
			for (String c: classNameList)
				for (Method m : Class.forName(c).getMethods())
					if (! liste.contains(m.getName()) && m.isDefault()) 
						liste.add(m.getName());
			return liste;
		}
		
		/* Partie 3 */
		public static ArrayList<String> methodesToAddToInterfaces(ArrayList<String>  classNameList, ArrayList<String>  listeMethodes) throws SecurityException, ClassNotFoundException {
			ArrayList<String> liste = new ArrayList<String>();
			for(String m : listeMethodes){
				Map<List<String>, List<Integer>> map = searchMethodesImplementedByManyClass(classNameList, m, 1);
				if(!map.isEmpty()) {
					liste.add(map.toString());
					System.out.println(map.toString());				
				}
			}
			//System.out.println(liste);
			return liste;
		}
		
		public static Map<List<String>, List<Integer>> searchMethodesImplementedByManyClass(ArrayList<String>  classNameList, String m, int numInter) throws SecurityException, ClassNotFoundException{
			int count = 0;
			Map<List<String>, List<Integer>> map = new HashMap<List<String>, List<Integer>>();
			ArrayList<Integer> liste = new ArrayList<Integer>();
			ArrayList<String> test = new ArrayList<String>();
			String cls = "";
			for(String c : classNameList){						
				if (signatures(c).contains(m)) {					
					count++;
					cls = c;
					//System.out.println(classNameList.indexOf(c));
					liste.add(classNameList.indexOf(c));
					test.add(c);
				}
			}
			
			if(count > 2) {
				ArrayList<String> interfaces = new ArrayList<String>();
				interfaces.add(implementNames(cls).toString());
				interfaces.add(m);
				map.put(interfaces, liste);			
			}
			return map;
		}
		
	
		
		
		public static void main(String[] args) throws SecurityException, ClassNotFoundException, IOException, IllegalArgumentException, IllegalAccessException {
			

			// on se concentre sur quelques classes
			// la première vient de l'API apache commons collections
			// qui a été au préalable ajoutée dans les librairies du projet sous eclipse
			// par le java build path
			// les autres sont des classes de l'API Java
			String[] listeDesClassesInterfaces = {"org.apache.commons.collections4.set.CompositeSet",
					 "org.apache.commons.collections4.set.ListOrderedSet",
					 "org.apache.commons.collections4.set.MapBackedSet",
					 "org.apache.commons.collections4.set.PredicatedNavigableSet",
					 "org.apache.commons.collections4.set.PredicatedSortedSet",
					 "org.apache.commons.collections4.set.PredicatedSet",
					 "org.apache.commons.collections4.set.TransformedNavigableSet",
					 "org.apache.commons.collections4.set.TransformedSet",
					 "org.apache.commons.collections4.set.TransformedSortedSet",
					 "org.apache.commons.collections4.set.UnmodifiableNavigableSet",
					 "org.apache.commons.collections4.set.UnmodifiableSet",
					 "org.apache.commons.collections4.set.UnmodifiableSortedSet"
						 
					 };
			
			
			//ajoutPrefixe("java.util.",listeDesClassesInterfaces);
			
			ArrayList<String> listeComplete = new ArrayList<>();
			listeComplete.addAll(Arrays.asList(listeDesClassesInterfaces));
					
			/* Create File containing the names of concrete classes*/
			// le répertoire resultat2017 doit exister dans votre projet
			//DataExtractionClassSet dFile = new DataExtractionClassSet();
			extractionInterface2017 dFile = new extractionInterface2017();
			dFile.setEntityList(concreteClassList(listeDesClassesInterfaces));
			createFileFromList("./resultats.txt", dFile.getEntityList());
			
		
			extractionInterface2017 eI = new extractionInterface2017();
			eI.setEntityList(concreteClassList(listeDesClassesInterfaces));
			eI.setCharacteristicList(signatureSet(eI.getEntityList()));
			
			System.out.println("Méthodes statiques: " + staticMethodes(eI.getEntityList()));
			System.out.println("Méthodes par défaut: " + defaultMethodes(eI.getEntityList()));
			System.out.println("Attributs public final static " + attributSet(eI.getEntityList(), "public static final"));
			System.out.println("{[[Hiérarchie d'Interfaces], méthode ] = [les indices de classes où cette méthode est implementée]}");
			methodesToAddToInterfaces(eI.getEntityList(), eI.getCharacteristicList());
			
			/* Create table for concrete classes signatures*/
			// cette table servira de fichier d'entree pour un outil dans un prochain TP
			// elle peut aider à comprendre quelle classe contient quelle signature de methode
			// le fichier csv peut etre ouvert dans open office (separateur '|') pour manipuler les colonnes
			// et trouver des groupes de signatures communes à des groupes de classes concretes
			
			ArrayList<String> classesConcretesAbstraites = new ArrayList<>();
			classesConcretesAbstraites.addAll(concreteAbstractClassList(listeDesClassesInterfaces));
			
			extractionInterface2017 d4 = new extractionInterface2017();
			d4.setEntityList(classesConcretesAbstraites);
			d4.setCharacteristicList(signatureSet(d4.getEntityList()));
			d4.createTable(2);
			d4.afficheTable();
			d4.ecrireTable("./format.rcft");
			d4.ecrireTable("./format.csv");}
		
			
		
		
	}


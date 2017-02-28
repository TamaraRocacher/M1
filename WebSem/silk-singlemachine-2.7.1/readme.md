Silk Single Machine can be used to generate RDF links on a single machine. Silk Single Machine is executed from the command line, for a graphical user interface use the Silk Workbench.

Prerequisites:

- Java Runtime Environment: The Silk Link Discovery Framework runs on top of the JVM. Get the most recent JRE.

What to do:

- Write a Silk-LSL configuration file to specify which resources should be interlinked.

- Run Silk Single Machine: java -DconfigFile=<Silk-LSL file> [-DlinkSpec=<Interlink ID>] [-Dthreads=<threads>]  [-DlogQueries=(true/false)] [-Dreload=(true/false)] -jar silk.jar

- Review Results: Open the output files designated in the Silk-LSL configuration and review the generated links.

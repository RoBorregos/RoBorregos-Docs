# NATURAL LANGUAGE PROCESSING (NLP)
**Natural language processing (NLP)** involves employing machine learning techniques to enable computers to understand, process, and interpret human language effectively. This technology plays a crucial role in action interpretation, bridging the gaps presented by dialects, slang terms, and grammatical nuances commonly encountered in everyday communication.
Natural language processing (NLP) NLP tasks involve the deconstruction of human text or speech into more manageable components that computer programs can easily interpret. Some of the capabilities can be:

**-	Word-sense disambiguation**, where some words may hold different meanings when used in different scenarios. 

**-	Named-entity recognition**, where it identifies unique names for people, places, events, companies, and other entities and establishes relationships between the different entities within a sentence. For example, in the sentence “Jane went on a vacation to France, and she wants to go again”, the NLP software would identify "Jane" and "France" as special entities. Additionally, it may employ co-reference resolution to determine if different words refer to the same entity. In the provided example, both "Jane" and "she" refer to the same person.

**-	Sentiment analysis,** where the emotion transmitted by textual data can be interpreted. NLP software analyzes text for words or phrases that show dissatisfaction, happiness, doubt, regret, and other hidden emotions.

In our case, after converting the audio input into a text string, our objective has been to use NLP to simplify the sentence and conduct an information extraction process. To achieve this, we have leveraged the GPT-3 API and developed a customized model enabling us to organize the input sentence into a common structure to work with. From there, we can analyze the most significant words to determine which type of command it is (manipulation, movement, introducing…) and then act accordingly.

### OUR MODEL
Using GPT-3 API, we have created a customized model that enables the separation of long sentences into smaller ones separated by commas and following the structure of **action + complement**.
For example: 
-	Input: “Bring me the Milk from the kitchen”.
-	Output: “go kitchen, bring milk”.
In that way, we can analyze the first and the second word separately and find the best possible approach.

### EMBEDDINGS
Embeddings are a natural language processing (NLP) technique that converts human language into mathematical vectors based on their inherent characteristics and associated categories. These vectors encapsulate the essence of words, facilitating more efficient language processing by computers. Through the mapping of words (or phrases) to numerical vectors in a high-dimensional space, embeddings ensure that similar words cluster together, thereby capturing their semantic and contextual relationships effectively.

As previously discussed, each desired sentence is broken down into an action + complement. Then, we use an embedding model provided by the GPT-3 API (*text-embedding-ada-002*) to assign a numerical vector to each of these words. Previously, we have constructed a Panda Dataframe with known words directly associated with common actions such as “go”, “grab”, “find”… . By comparing the embeddings of known words with those of detected words, we can assess their spatial relationship. If they are located close together, meaning their similarity is high upon analysis, we can assume they refer to the same concept. This allows us to proceed with the routine assuming it is that word, enabling the use of different words to express the same meaning. For example, “go” and “walk” should be really close spatially, so if we know that “go” is a movement action, “walk” will be too. 

### PANDAS DATAFRAMES
Pandas is a powerful open-source Python library that provides high-performance data manipulation and analysis tools. Pandas introduces data structures like DataFrames and Series, which allow users to work with labeled and relational data effortlessly. With its rich functionality, Pandas enables users to perform operations such as indexing, slicing, aggregating, and merging data sets efficiently. 

In the context of Robocup Competition, we are using Pandas DataFrames to organize words related to the competition. That is, we have created several dataframes in which the actions, places, names and items are organized. In each of them, there are different columns with the element, their specfic category (if applicable) and, the most important thing, their embedding conification. For example,
-	Dataframe “items”
-	Item “apple”
-	Specific category “food”
-	Embedding: it will be a mathematical vector of 1536 length

### PRODUCT SIMILARITIES
Firstly, a similarity search is conducted with the verb through the embedding process, followed by a repetition of the process for the complement. To make the process optimal and logical, the complement's similarity search is performed based on the detected main action. This means that, depending on the identified main action, the similarity search for the complement will be carried out only in a specific DataFrame:
-	For "go" or "put", the search is conducted in the location DataFrame.
-	For "grab", the search is performed in the elements DataFrame.
-	For "find", the search is executed in the names DataFrame.
-	For “introduce” it is not necessary to search anywhere because it is the only action without complement needed.

Now, two scenarios may occur:
1. If the complement is a general category, such as "drinks," a list with all possible drinks will be returned. It will then be the responsibility of the vision department to choose the most suitable option.
2. If the complement is specific, like "milk," then there will be no options available so only one thing will be returned. 

### FUTURE WORK
Some enhancements aimed at making the system more automated and robust include the following:

Firstly, there's a desire to automate the process of creating DataFrames with known elements. Currently, there are DataFrames created with actions, locations, items and names published in RoboCupAtHome/gpsr_command_generator. However, if a new category is added, the process of integrating it into the system should be faster and simpler.

Another aspect to implement is adding contextual meaning within the similarity analysis. For instance, if the user is instructing the robot to move to the kitchen, it's likely that they are seeking for food, not items from the bathroom like toothpaste. By incorporating this context, the list of possibilities can be further narrowed down, ensuring more efficient traversal without the risk of performing nonsensical actions.


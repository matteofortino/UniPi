# text_analyzer.py
"""
Module for analyzing text documents.
This demonstrates the use of Python's string and collections modules from the standard library,
applying fundamental Natural Language Processing (NLP) concepts such as tokenization,
frequency analysis, and readability evaluation.

TODO: Import the necessary modules from the standard library
"""
# Import required modules here
import string
import collections
import re

# Common English stopwords (words that are very frequent but carry little meaning)
STOPWORDS_EN = {
    "the", "a", "an", "is", "in", "to", "and", "of", "it", "for",
    "on", "with", "that", "this", "at", "from", "or", "be", "are",
    "was", "were", "but", "not", "have", "has", "had", "do", "does",
    "did", "will", "would", "can", "could", "should", "may", "might"
}


def count_statistics(text):
    """
    Count basic statistics about a text.

    Args:
        text (str): The text to analyze.

    Returns:
        tuple: A tuple containing (n_words, n_sentences, n_characters) where:
               - n_words is the number of words
               - n_sentences is the number of sentences
               - n_characters is the number of characters (excluding spaces)
    """
    # TODO: Implement this function
    if not text:
        return (0,0,0)
    # Hint: Use split() to count words
    words = text.split()
    n_words = len(words)

    # Hint: Count sentence-ending punctuation marks (. ! ?) to count sentences
    n_sentences = sum(1 for ch in text if ch in ".?!")
    # Hint: If there are no sentence-ending marks but text is not empty, count as 1 sentence
    if n_words > 0 and n_sentences == 0:
        n_sentences = 1

    # Hint: Count characters excluding spaces
    n_characters = len(text.replace(" ",""))
    return (n_words, n_sentences, n_characters)


def word_frequency(text, top_n=10, ignore_stopwords=True):
    """
    Analyze word frequency in a text.

    Args:
        text (str): The text to analyze.
        top_n (int): Number of most frequent words to return.
        ignore_stopwords (bool): Whether to exclude common stopwords.

    Returns:
        list: A list of tuples (word, count) sorted by frequency in descending order.
    """
    # TODO: Implement this function
    # 1. Remove punctuation from the text (use string.punctuation and str.maketrans/str.translate)
    table = str.maketrans("", "", string.punctuation)
    clean_text = text.translate(table)
    # 2. Convert to lowercase and split into words
    words = clean_text.lower().split()
    # 3. Filter out stopwords if ignore_stopwords is True (use the STOPWORDS set above)
    if ignore_stopwords:
        words = [word for word in words if word not in STOPWORDS_EN]
    # 4. Count word frequencies (use collections.Counter)
    counter = collections.Counter(words)
    # 5. Return the top_n most common words (use .most_common())
    return counter.most_common(top_n)


def evaluate_readability(text):
    """
    Evaluate the readability and complexity of a text.

    Args:
        text (str): The text to evaluate.

    Returns:
        tuple: A tuple containing (score, level, feedback) where:
               - score is an integer from 1-5 (1 = very simple, 5 = very complex)
               - level is a string like "Simple", "Moderate", "Complex", etc.
               - feedback is a string explaining the evaluation
    """
    # TODO: Implement this function
    words = text.split()
    n_words = len(words)

    table = str.maketrans("", "", string.punctuation)
    clean_text = text.translate(table)
    # 1. Calculate the average word length (longer words = more complex)
    avg_word_length = len(clean_text) / n_words

    # 2. Calculate the average sentence length in words (longer sentences = more complex)
    n_sentences = sum(1 for ch in text if ch in ".?!")
    if n_sentences == 0 and n_words > 0:
        n_sentences = 1
    avg_sentence_length = len(clean_text) / n_sentences

    # 3. Calculate vocabulary richness (unique words / total words)
    unique_words = set(word.lower() for word in words)
    n_unique_words = len(unique_words)
    vocab_richness = n_unique_words / n_words

    # 4. Assign a score based on these metrics
    word_score = 0
    if avg_word_length >= 7:
        word_score = 2
    elif avg_word_length >= 5:
        word_score = 1

    sentence_score = 0
    if avg_sentence_length >= 20:
        sentence_score = 2
    elif avg_sentence_length >= 12:
        sentence_score = 1

    vocab_score = 0
    if vocab_richness >= 0.85:
        vocab_score = 2
    elif vocab_richness >= 0.65:
        vocab_score = 1

    total = word_score + sentence_score + vocab_score  # range: 0-6

    # Map total (0-6) to score (1-5)
    score = 0
    if total <= 1:
        score = 1
    elif total == 2:
        score = 2
    elif total == 3:
        score = 3
    elif total <= 5:
        score = 4
    else:
        score = 5
    levels = {1: "Very Simple", 2: "Simple", 3: "Moderate", 4: "Complex", 5: "Very Complex"}
    level = levels[score]

    # 5. Provide appropriate feedback
    parts = []

    # Word length feedback
    if avg_word_length >= 7:
        parts.append("Average word length is high, indicating complex vocabulary.")
    elif avg_word_length >= 5:
        parts.append("Words are of moderate length.")
    else:
        parts.append("Words are short and simple.")

    # Sentence length feedback
    if avg_sentence_length >= 20:
        parts.append("Sentences are long and complex.")
    elif avg_sentence_length >= 12:
        parts.append("Sentences are of moderate length.")
    else:
        parts.append("Sentences are short and easy to read.")

    # Vocabulary richness feedback
    if vocab_richness >= 0.85:
        parts.append("Vocabulary richness is high, with many unique words.")
    elif vocab_richness >= 0.65:
        parts.append("Vocabulary richness is moderate.")
    else:
        parts.append("Vocabulary is repetitive with few unique words.")

    feedback = " ".join(parts)
    return (score, level, feedback)

def extract_key_sentences(text, n=3):
    """
    Return the n most important sentences based on word frequency (extractive summarization).

    Args:
        text (str): The text to summarize.
        n (int): Number of key sentences to return.

    Returns:
        list: A list of the n most important sentences as strings.
    """
    sentences = re.split(r"[.!?]+", text)
    n_word, _, _ = count_statistics(text)
    word_freq = dict(word_frequency(text, top_n=n_word))

    sentences_score = []
    for sentence in sentences:
        words = sentence.split()
        score = sum(word_freq[word] for word in words)
        sentences_score.append((sentence, score))

    

    sentences_score.sort(key=lambda x: x[1], reverse=True) 
    sentences = [sentence for sentence, _ in sentences_score]
    return sentences[:n]


def detenct_language(text):
    """
    Guess whether a text is in English or Italian based on stopword frequency.

    Args:
        text (str): The text to analyze.

    Returns:
        str: 'English', 'Italian', or 'Unknown'.
    """
    table = str.maketrans("","",string.punctuation)
    cleant_text = text.translate(table).lower()
    words = cleant_text.split()

    STOPWORDS_IT = {
        "il", "lo", "la", "i", "gli", "le", "un", "uno", "una",
        "e", "è", "di", "a", "da", "in", "con", "su", "per",
        "tra", "fra", "che", "non", "si", "come", "ma", "se",
        "sono", "ha", "ho", "hai", "hanno", "essere", "questo",
        "questa", "dei", "del", "della", "delle", "degli"
    }

    english_hits = sum(1 for word in words if word in STOPWORDS_EN)
    italian_hits = sum(1 for word in words if word in STOPWORDS_IT)

    english_ratio = english_hits / len(words)
    italian_ratio = italian_hits / len(words)

    if english_ratio == 0 and italian_ratio == 0:
        return "Unknown"
    if english_ratio >= italian_ratio:
        return "English"
    return "Italian"





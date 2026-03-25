# test_text_analyzer_unit.py
"""
Unit tests for the text_analyzer module.
Run with: python -m unittest test_text_analyzer_unit.py
"""
import unittest
import text_analyzer as ta


class TestTextAnalyzer(unittest.TestCase):

    # --- count_statistics tests ---

    def test_count_statistics_words(self):
        """Test that word count is correct."""
        text = "The quick brown fox jumps over the lazy dog"
        n_words, _, _ = ta.count_statistics(text)
        self.assertEqual(n_words, 9, "Word count should be 9")

    def test_count_statistics_sentences(self):
        """Test that sentence count is correct."""
        text = "Hello world. How are you? I am fine!"
        _, n_sentences, _ = ta.count_statistics(text)
        self.assertEqual(n_sentences, 3, "Sentence count should be 3")

    def test_count_statistics_sentences_no_punctuation(self):
        """Test that text without sentence-ending punctuation counts as 1 sentence."""
        text = "Hello world this is a test"
        _, n_sentences, _ = ta.count_statistics(text)
        self.assertEqual(n_sentences, 1,
                         "Text without sentence-ending punctuation should count as 1 sentence")

    def test_count_statistics_characters(self):
        """Test that character count excludes spaces."""
        text = "Hello world"
        _, _, n_characters = ta.count_statistics(text)
        self.assertEqual(n_characters, 10,
                         "Character count should exclude spaces (10 for 'Hello world')")

    # --- word_frequency tests ---

    def test_word_frequency_top_n(self):
        """Test that word_frequency returns exactly top_n results."""
        text = "apple banana apple cherry banana apple date cherry banana"
        result = ta.word_frequency(text, top_n=3, ignore_stopwords=False)
        self.assertEqual(len(result), 3,
                         "Should return exactly 3 results when top_n=3")

    def test_word_frequency_order(self):
        """Test that results are sorted by frequency in descending order."""
        text = "apple banana apple cherry banana apple"
        result = ta.word_frequency(text, top_n=3, ignore_stopwords=False)
        self.assertEqual(result[0][0], "apple",
                         "Most frequent word should be 'apple'")
        self.assertEqual(result[0][1], 3,
                         "'apple' should appear 3 times")
        self.assertEqual(result[1][0], "banana",
                         "Second most frequent word should be 'banana'")

    def test_word_frequency_ignore_stopwords(self):
        """Test that stopwords are excluded when ignore_stopwords is True."""
        text = "the cat is on the mat and the cat is happy"
        result = ta.word_frequency(text, top_n=10, ignore_stopwords=True)
        result_words = [word for word, count in result]
        self.assertNotIn("the", result_words,
                         "'the' should be excluded when ignoring stopwords")
        self.assertNotIn("is", result_words,
                         "'is' should be excluded when ignoring stopwords")
        self.assertIn("cat", result_words,
                      "'cat' should be included (not a stopword)")

    def test_word_frequency_include_stopwords(self):
        """Test that stopwords are included when ignore_stopwords is False."""
        text = "the cat is on the mat and the cat is happy"
        result = ta.word_frequency(text, top_n=10, ignore_stopwords=False)
        result_words = [word for word, count in result]
        self.assertIn("the", result_words,
                      "'the' should be included when not ignoring stopwords")

    def test_word_frequency_case_insensitive(self):
        """Test that words are counted case-insensitively."""
        text = "Apple apple APPLE"
        result = ta.word_frequency(text, top_n=1, ignore_stopwords=False)
        self.assertEqual(result[0][1], 3,
                         "'Apple', 'apple', and 'APPLE' should be counted as the same word")

    def test_word_frequency_punctuation(self):
        """Test that punctuation is removed from words."""
        text = "hello, world! hello. world?"
        result = ta.word_frequency(text, top_n=2, ignore_stopwords=False)
        result_words = [word for word, count in result]
        self.assertIn("hello", result_words,
                      "'hello' should appear without punctuation")
        self.assertIn("world", result_words,
                      "'world' should appear without punctuation")
        self.assertEqual(result[0][1], 2,
                         "Each word should appear 2 times after removing punctuation")

    # --- evaluate_readability tests ---

    def test_evaluate_readability_simple(self):
        """Test evaluation of simple text."""
        text = "I run. He sits. We go. She eats. They play."
        score, level, _ = ta.evaluate_readability(text)
        self.assertLessEqual(score, 2,
                             "Score for simple text should be low")
        self.assertIn("simple", level.lower(),
                      "Level should indicate simplicity")

    def test_evaluate_readability_complex(self):
        """Test evaluation of complex text."""
        text = ("The extraordinary implications of contemporary philosophical "
                "epistemology necessitate a comprehensive understanding of "
                "sophisticated methodological frameworks that fundamentally "
                "characterize the interdisciplinary investigation of "
                "phenomenological consciousness throughout civilization.")
        score, level, _ = ta.evaluate_readability(text)
        self.assertGreaterEqual(score, 4,
                                "Score for complex text should be high")
        self.assertIn("complex", level.lower(),
                      "Level should indicate complexity")

    def test_evaluate_readability_consistency(self):
        """Test that the same text always gets the same evaluation."""
        text = "Python is a great programming language. It is easy to learn."
        eval1 = ta.evaluate_readability(text)
        eval2 = ta.evaluate_readability(text)
        self.assertEqual(eval1, eval2,
                         "Same text should get consistent evaluation")

    def test_evaluate_readability_feedback(self):
        """Test that readability feedback mentions relevant aspects."""
        # Simple short text
        text = "I run. He sits. We go."
        _, _, feedback = ta.evaluate_readability(text)
        self.assertTrue(
            "short" in feedback.lower() or
            "simple" in feedback.lower() or
            "easy" in feedback.lower(),
            "Feedback for simple text should mention simplicity"
        )

        # Complex text
        text = ("The extraordinary implications of contemporary philosophical "
                "epistemology necessitate a comprehensive understanding of "
                "sophisticated methodological frameworks.")
        _, _, feedback = ta.evaluate_readability(text)
        self.assertTrue(
            "long" in feedback.lower() or
            "complex" in feedback.lower() or
            "high" in feedback.lower(),
            "Feedback for complex text should mention complexity"
        )


if __name__ == '__main__':
    unittest.main()

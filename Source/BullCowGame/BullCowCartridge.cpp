// Fill out your copyright notice in the Description page of Project Settings.
#include "BullCowCartridge.h"
#include "HiddenWordList.h"
#include "Math/UnrealMathUtility.h"

void UBullCowCartridge::BeginPlay() // When the game starts
{
    Super::BeginPlay();
    Isograms = GetValidWords(Words); // Load the words that are Isograms in a TArray <FString>
    SetupGame(); // It lays all the groundwork to start playing
}

void UBullCowCartridge::OnInput(const FString& PlayerInput) // When the player hits enter
{
    if(bGameOver) // When its true, clear the screen and restart everything to play again
    {
        ClearScreen();
        SetupGame();
        return;
    }
    else // if not yet finished, processes the player's input to validate whether it is correct or not
    {
        ProcessGuess(PlayerInput);
    }
}
void UBullCowCartridge::SetupGame()
{
    HiddenWord = Isograms[FMath::RandRange(0, Isograms.Num() - 1)]; // Pick one word of the Isograms between all posible options that were previously validated
    Lives = HiddenWord.Len(); // You have as many lives as letters of the isogram you have to guess
    bGameOver = false; // Its impossible that you start playing with the game over

    // Welcome message and some hints for the player
    PrintLine(TEXT("Welcome to Bull Cow Game!"));
    PrintLine(TEXT("You have %i lives"), Lives);
    PrintLine(TEXT("The character 1 of the Hidden Word is: %c"), HiddenWord[0]);
    PrintLine(TEXT("The 4 character of the Hidden Word is: %c"), HiddenWord[3]);
    // DEBUG LINE -> PrintLine(TEXT("The Hidden Word is: %s"), *HiddenWord);
    PrintLine(TEXT("Type in your guess and press <Enter>\nto continue..."));
}

void UBullCowCartridge::EndGame() // We call this function when the player loses all his lifes
{
    bGameOver = true;
    PrintLine(TEXT("Press <Enter> to play again\n"));
}

void UBullCowCartridge::ProcessGuess(const FString& Guess) // Checking if the Guess is equal to the HiddenWord
{
    if (Guess == HiddenWord) // If it is equal, you win and the game ends
    {
        PrintLine(TEXT("You win!")); 
        EndGame();
        return;
    }

    if (Guess.Len() != HiddenWord.Len()) //We only lose a life when the guess is the same long as the HiddenWord
    {
        PrintLine(TEXT("Sorry, try guessing again!\nYou have %i lives remaining"), Lives);
        return;
    }    

    if (!IsIsogram(Guess)) // if the player does not enter an isogram, he is warned and is not punished in any way.
    {
        PrintLine(TEXT("No repeating letters, guess again!"));
        return;
    }

    // Lost a life in other cases
    PrintLine(TEXT("Lost a life!"));
    --Lives;
    
    if (Lives <= 0) // Check for end the game
    {
        ClearScreen();
        PrintLine(TEXT("You have no lives left!\n"));
        PrintLine(TEXT("The hidden word was: %s\n"), *HiddenWord);
        EndGame();
        return;
    }

    FBullCowCount Count; // Instantiating the structure that stores the bulls and cows
    GetBullCows(Guess, Count); // Once the player's input has been processed, we determine how many bulls and cows are in his input.
    PrintLine(TEXT("You have %i Bulls and %i Cows"), Count.Bulls, Count.Cows);
    PrintLine(TEXT("Guess again. You have %i lives remaining"), Lives);
}

bool UBullCowCartridge::IsIsogram(const FString& Word) const // Check that any word has reppeating letters
{
    bool bIsogram;
    int32 Index = 0, Comparison = 0;        
    for(;Index < Word.Len() - 1; Index += 1)
    {
    for(;Comparison < Word.Len(); Comparison += 1) //Compare the position [Index] of the word with all others letters
        {
            if (Word[Index] == Word[Comparison])
            {
                bIsogram = false;
            }
            else
            {
                bIsogram = true;
            }
        }
    }
    return bIsogram;
}

//We obtain a TArray <FString> with words that are isograms and have 4 or more letters, but no more than 8.
TArray<FString> UBullCowCartridge::GetValidWords(const TArray<FString>& WordList) const 
{
    TArray<FString> ValidWords;

    for(FString TempWord : WordList)
    {    
    if (TempWord.Len() >= 4 && TempWord.Len() <= 8 && IsIsogram(TempWord))
        {
            ValidWords.Emplace(TempWord); // Emplace allow us to add words at the end of the TArray (most eficient way)
        }
    }
    return ValidWords;
}

FBullCowCount UBullCowCartridge::GetBullCows(const FString& Guess, FBullCowCount& CountP) const
{
    /*
    The Bulls counter increments when the value in the index of the Guess FString is equal to 
    the value stored in the same index of the HiddenWord FString.
    
    Example.    Down we have glow as Guess value and garden as HiddenWord value,
                the first letter of both words are g, same position = Bull
    Guess       -> glow
                   |
    HiddenWord  -> garden

    The Cows counter increases when any Guess value that is not in the same position as HiddenWord
    equals.

    Example.    We can see that the letters 'e', 'a' and 'k' appear in both words, but in different
                indexes, so we have 3 Cows

                   0 1 2 3 4
    Guess       -> s t e a k
                   
                   0 1 2 3
    HiddenWord  -> c a k e
    */
    for (int32 GuessIndex = 0; GuessIndex < Guess.Len(); GuessIndex++)
    {
        if (Guess[GuessIndex] == HiddenWord[GuessIndex])
        {
            CountP.Bulls += 1;
            continue; // If we found one Bull, we don't need to iterate in the other letters
        }
        for (int32 Index = 0; Index < HiddenWord.Len(); Index++)
        {
            if (Guess[GuessIndex] == HiddenWord[Index])
            {
                CountP.Cows += 1;
                break;  // If we found a Cow, we never will found another one equal because an isogram dont repeat letters,
                        // thats the reason for break the loop and not continue to next iterations
            }
        }
    }
    return CountP;
}
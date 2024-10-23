#pragma once

#define print(Text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, TEXT(Text))
#define printk(Key ,Text) if (GEngine) GEngine->AddOnScreenDebugMessage(Key, 3.f, FColor::Blue, TEXT(Text))
#define printf(Format, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, FString::Printf(TEXT(Format), ##__VA_ARGS__))
#define printfc(Key, Color, Format, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(Key, 3.f, Color, FString::Printf(TEXT(Format), ##__VA_ARGS__))

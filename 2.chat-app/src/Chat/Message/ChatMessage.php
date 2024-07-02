<?php

namespace App\Chat\Message;

final class ChatMessage extends Message
{
    public function __construct(public readonly string $name, string $message)
    {
        parent::__construct($message);
    }

    public function __toString()
    {
        return "$this->name: $this->message";
    }
}
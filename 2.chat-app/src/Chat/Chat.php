<?php

namespace App\Chat;

use App\Chat\Message\ChatMessage;
use PhpTui\Term\Event;
use PhpTui\Term\Event\CharKeyEvent;
use PhpTui\Term\Event\CodedKeyEvent;
use PhpTui\Term\KeyCode;
use PhpTui\Tui\Extension\Core\Widget\BlockWidget;
use PhpTui\Tui\Extension\Core\Widget\GridWidget;
use PhpTui\Tui\Extension\Core\Widget\List\ListItem;
use PhpTui\Tui\Extension\Core\Widget\List\ListState;
use PhpTui\Tui\Extension\Core\Widget\ListWidget;
use PhpTui\Tui\Extension\Core\Widget\ParagraphWidget;
use PhpTui\Tui\Model\Layout\Constraint;
use PhpTui\Tui\Model\Modifier;
use PhpTui\Tui\Model\Style;
use PhpTui\Tui\Model\Text\Title;
use PhpTui\Tui\Model\Widget;
use PhpTui\Tui\Model\Widget\Borders;


final class Chat
{

    private int $selected = 0;
    private ListState $state;

    private string $prompt = "";
    private array $messages = [];

    /**
     * @var false|resource|\Socket
     */
    private $socket;

    private function __construct(
        public string $address,
        public int $port
    )
    {
        $this->state = new ListState();

        // ここでソケットを初期化する
    }

    public static function new(string $address, int $port): self
    {
        return new self($address, $port);
    }

    public function build(): Widget
    {
        // ここでメッセージを受け取る

        return
            BlockWidget::default()->borders(Borders::ALL)
                ->style(Style::default()->white())
                ->titles(Title::fromString("messages"))
                ->widget(
                    GridWidget::default()
                        ->constraints(Constraint::percentage(100), Constraint::min(3))
                        ->widgets(
                            $this->messageList(),
                            $this->textarea()
                        )
                );
    }

    private function messageList(): Widget
    {
        return ListWidget::default()
            ->state($this->state)
            ->highlightSymbol("")
            ->highlightStyle(Style::default()->white()->onLightCyan())
            ->select($this->selected)
            ->items(...array_map(fn(string $message) => ListItem::fromString($message), $this->messages));
    }

    private function textarea(): Widget
    {
        return BlockWidget::default()
            ->borders(Borders::ALL)
            ->widget(
                ParagraphWidget::fromString($this->prompt)
            );
    }

    public function handle(Event $event): void
    {
        if ($event instanceof CodedKeyEvent) {
            if ($event->code === KeyCode::Down) {
                $this->selected++;

                $upto = count($this->messages);
                if ($this->selected == $upto) {
                    $this->selected = $upto - 1;
                }

                return;
            }

            if ($event->code === KeyCode::Up) {
                $this->selected--;

                if ($this->selected < 0) {
                    $this->selected = 0;
                }

                return;
            }

            if ($event->code === KeyCode::Enter) {
                $this->messages[] = new ChatMessage("me", $this->prompt);
                $this->prompt = "";
                $this->selected = count($this->messages) - 1;
                return;
            }

            if ($event->code === KeyCode::Backspace) {
                $this->prompt = substr($this->prompt, 0, strlen($this->prompt) - 1);
                return;
            }
        } elseif ($event instanceof CharKeyEvent) {
            if ($event->modifiers === Modifier::NONE) {
                $this->prompt .= $event->char;
            }
        }
    }
}

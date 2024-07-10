<?php

namespace App;

use App\Chat\Chat;
use PhpTui\Term\Actions;
use PhpTui\Term\ClearType;
use PhpTui\Term\Event\CharKeyEvent;
use PhpTui\Term\KeyModifiers;
use PhpTui\Term\Terminal;
use PhpTui\Tui\Bridge\PhpTerm\PhpTermBackend;
use PhpTui\Tui\DisplayBuilder;
use PhpTui\Tui\Extension\Core\Widget\GridWidget;
use PhpTui\Tui\Extension\Core\Widget\ParagraphWidget;
use PhpTui\Tui\Model\Display\Backend;
use PhpTui\Tui\Model\Display\Display;
use PhpTui\Tui\Model\Layout\Constraint;
use PhpTui\Tui\Model\Text\Text;
use PhpTui\Tui\Model\Widget;

final class App
{
    private function __construct(
        private readonly Terminal $terminal,
        private readonly Display  $display,
        private readonly ?Chat $chat = null
    )
    {

    }

    public static function new(?Terminal $terminal = null, ?Backend $backend = null): self
    {
        $terminal ??= Terminal::new();
        $display = DisplayBuilder::default($backend ?? PhpTermBackend::new($terminal))->build();

        return new self(
            $terminal,
            $display,
            Chat::new("0.0.0.0", 9600)
        );
    }

    public function run(): int
    {
        try {
            $this->boot();
            return $this->doRun();
        } finally {
            $this->shutdown();
        }
    }

    private function doRun(): int
    {
        while (true) {
            while (null !== $event = $this->terminal->events()->next()) {
                if ($event instanceof CharKeyEvent) {
                    if ($event->modifiers === KeyModifiers::CONTROL) {
                        if ($event->char === 'q') {
                            break 2;
                        }
                    }
                }

                $this->chat->handle($event);
            }

            $this->display->draw($this->layout());

            usleep(5_000);
        }

        return 0;
    }

    private function layout(): Widget
    {
        return GridWidget::default()
            ->constraints(
                Constraint::min(1),
                Constraint::percentage(100),
                Constraint::min(1),
            )->widgets(
                $this->header(),
                $this->chat->build(),
                $this->footer()
            );
    }

    private function header(): Widget
    {
        return ParagraphWidget::fromString("UDP Broadcast Chat System");
    }

    private function footer(): Widget
    {
        return ParagraphWidget::fromText(Text::parse("<fg=red>Ctrl + \<q></> exit"));
    }

    private function boot(): void
    {
        $this->terminal->execute(Actions::cursorHide());
        $this->terminal->execute(Actions::alternateScreenEnable());
        $this->terminal->enableRawMode();
    }

    private function shutdown(): void
    {
        $this->chat->shutdown();
        $this->terminal->disableRawMode();
        $this->terminal->execute(Actions::alternateScreenDisable());
        $this->terminal->execute(Actions::cursorShow());
        $this->terminal->execute(Actions::clear(ClearType::All));
    }
}
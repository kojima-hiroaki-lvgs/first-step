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
    private string $myname = "";
    private string $prompt = "";
    private array $messages = ["ENTER YOUR NAME BELOW"];
    private string $interface = "en0";
    private string $group = "239.0.0.1";
    private $recv_addr ;
    private string $flag = "";
    private array $info = [];
    private string $data = "";

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
        $this->socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
        socket_set_nonblock($this->socket); //非ブロッキング
        socket_set_option($this->socket, SOL_SOCKET, SO_BROADCAST, 1); //ブロードキャスト
       /* 
        socket_set_option($this->socket, IPPROTO_IP, IP_MULTICAST_IF, $this->interface); //IPv4 マルチキャストパケットの送信インターフェイス
        socket_set_option($this->socket, IPPROTO_IP, IP_MULTICAST_LOOP, 0); //マルチキャストのデータをループバックさせる
        socket_set_option($this->socket, IPPROTO_IP, MCAST_JOIN_GROUP, [
            'group' => $this->group,
            'interface' => $this->interface,
        ]); //マルチキャストグループに参加
        */
      
        // ここでソケットを初期化する
        socket_bind($this->socket, '0.0.0.0', $this->port); //ソケットに名前をつける、受信先の制限
        //socket_close($socket)

    }

    public static function new(string $address, int $port): self
    {
        return new self($address, $port);
    }

    public function build(): Widget
    {
        // ここでメッセージを受け取る

        if(socket_recvfrom($this->socket, $this->data, 94,0, $this->recv_addr , $this->port)){
        
        if(substr($this->data,0,2 ) == "00"){
            $this->flag = "01";
            $text = $this->flag . $this->myname;
            $this->info[$this->recv_addr] = substr($this->data, 2);
            if($this->recv_addr !== '10.152.0.178'){
            $this->messages[] = new ChatMessage(  $this->info[$this->recv_addr] , "has logged in") ; 
            socket_sendto($this->socket, $text, 94, 0, "255.255.255.255", $this->port);
            }
        } 
            
        if(substr($this->data,0,2) == "01"){
            if(array_key_exists($this->recv_addr, $this->info)){
            // infoにrecvAddrがあるとき
            $this->messages[] = new ChatMessage( $this->info[$this->recv_addr].":", "still here") ;
            } else {
            // infoにrecvAddrがないとき
            $this->info[$this->recv_addr] = substr($this->data, 2);
            $this->messages[] = new ChatMessage( $this->info[$this->recv_addr].":", "hello");
            }
        } 

        if(substr($this->data,0,2) == "02"){
            //echo $this->info[$this->recv_addr];
            if(array_key_exists($this->recv_addr, $this->info)){
                if($this->recv_addr !== '10.152.0.115'){
                $this->messages[] = new ChatMessage( $this->info[$this->recv_addr].":" , substr($this->data,2) ); 
                } //相手のメッセージを表示
            } else {
                $this->messages[] = new ChatMessage( "" , "unknown user has sent a message.") ;
            }
        }

        if(substr($this->data,0,2) == "03"){
            if(array_key_exists($this->recv_addr, $this->info)){
                $this->messages[] = new ChatMessage( $this->info[$this->recv_addr] , "has logged out" ); 
            } else {
                $this->messages[] = new ChatMessage( "" , "unknown user has logged out" ); 
            }
            unset($this->recv_addr);
        }
    }
        return
            BlockWidget::default()->borders(Borders::ALL)
                ->style(Style::default()->white())
                ->titles(Title::fromString($this->myname))
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

    public function shutdown() {
        $this->flag = "03";
            socket_sendto($this->socket, $this->flag, 1200, 0, "255.255.255.255", $this->port);
            $this->info = array();
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

            if($this->myname == ""){
                $this->flag = "00"; //名前を入力した時に入室を知らせる_00
                $this->myname = $this->prompt;
                $text = $this->flag . $this->prompt;
                $this->messages[] = new ChatMessage( "" ,"名前入力完了") ;
                socket_sendto($this->socket, "$text", 94, 0, "255.255.255.255", $this->port);
                $this->prompt = "";
                return;
                } 
                 
                //自分がメッセージを送信_02
                $this->flag = "02";
                $this->messages[] = new ChatMessage( "me:", $this->prompt);
                $text = $this->flag . $this->prompt;
                socket_sendto($this->socket, $text, 94, 0, "255.255.255.255", $this->port);
                $this->prompt = "";
                $this->selected = count($this->messages) - 1;
                return;            
            }
            
            

            if ($event->code === KeyCode::Backspace) {
                $this->prompt = substr($this->prompt, 0, strlen($this->prompt) - 1);
                return;
            }   }

            elseif ($event instanceof CharKeyEvent) {
                     if ($event->modifiers === Modifier::NONE) {
                     $this->prompt .= $event->char;
            }
            }   
    }
}


设计理念 for this component - update on the go

1. 选择不保留component, 而是采用component Manager, 将component归于游戏类, 而componentManager来负责管理所有的component
2. 不实现System, system的具体实现是由游戏中需要完成的事情决定的 - 降低耦合度